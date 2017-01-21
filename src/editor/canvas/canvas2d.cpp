#include "canvas2d.h"
#include "../document.h"
#include "../framework.h"
#include "../glwidget.h"
#include "../hierarchy.h"
#include "../project_mgr.h"
#include "../properties/property_param.h"
#include "../resource.h"
#include "../resource_mgr.h"
#include "../resource_mime.h"
#include "../status_bar.h"

#include "../tools/file_tools.h"
#include "../tools/json_tools.h"
#include "../tools/log_tool.h"
#include "../tools/math_tool.h"

#include "runtime/object_loader.h"

#include "../command/cmd_drop_files.h"
#include "../command/cmd_delete_node.h"
#include "../command/cmd_property_change.h"

#include <2d/CCNode.h>
#include <2d/CCDrawNode.h>
#include <2d/CCScene.h>
#include <2d/CCLayer.h>
#include <2d/CCSprite.h>
#include <base/CCDirector.h>
#include <renderer/CCTextureCache.h>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QMimeData>

#include <math.h>

const float OUT_BOUNDER     = 10.0f;
const float MATH_PI         = 3.14159265f;
const float MIN_RECT_WIDTH  = 10.0f;
const float MIN_RECT_HEIGHT = 10.0f;

namespace Editor
{
    namespace
    {
        const float KeyboardMoveDelta = 1.0f;

        cocos2d::Node* _findNodeByPoint(cocos2d::Node* reference, const cocos2d::Point & pt)
        {
            if(!reference->isVisible())
            {
                return NULL;
            }
            if(Framework::instance()->document_->findNodeConfig(reference).isNull())
            {
                return NULL;
            }

            cocos2d::Point localPt = cocos2d::PointApplyAffineTransform(pt, reference->getParentToNodeAffineTransform());

            auto & children = reference->getChildren();
            for(auto it = children.rbegin(); it != children.rend(); ++it)
            {
                cocos2d::Node* selectedChild = _findNodeByPoint(*it, localPt);
                if(NULL != selectedChild)
                {
                    return selectedChild;
                }
            }

            if(localPt.x >= 0 && localPt.y >= 0 &&
               localPt.x <= reference->getContentSize().width &&
               localPt.y <= reference->getContentSize().height)
            {
                return reference;
            }
            return NULL;
        }

        cocos2d::Node* findNodeByPoint(cocos2d::Node* reference, const cocos2d::Point & pt)
        {
            cocos2d::Point localPt = pt;
            if(reference->getParent() != nullptr)
            {
                localPt = reference->getParent()->convertToNodeSpace(localPt);
            }
            return _findNodeByPoint(reference, localPt);
        }

        void expandRect(cocos2d::Rect &rect, float x, float y)
        {
            rect.origin.x -= x;
            rect.origin.y -= y;
            rect.size.width += x * 2.0f;
            rect.size.height += y * 2.0f;
        }

        void expandRect(cocos2d::Rect &rect, float left, float bottom, float right, float top)
        {
            rect.origin.x -= left;
            rect.origin.y -= bottom;
            rect.size.width += left + right;
            rect.size.height += bottom + top;
        }

        cocos2d::Vec2 getAffineTransformScale(const cocos2d::AffineTransform &m)
        {
            cocos2d::Vec2 scale;
            scale.x = std::sqrt(m.a * m.a + m.b * m.b);
            scale.y = std::sqrt(m.c * m.c + m.d * m.d);
            return scale;
        }

        cocos2d::Rect getNodeDragRect(cocos2d::Node *node)
        {
            cocos2d::Rect rect;

            rect.size.width = std::max(node->getContentSize().width, MIN_RECT_WIDTH);
            rect.size.height = std::max(node->getContentSize().height, MIN_RECT_HEIGHT);
            return rect;
        }

        void rectToPoints(cocos2d::Point *points, const cocos2d::Rect &rect)
        {
            points[0].setPoint(rect.getMinX(), rect.getMinY()); // left bottom
            points[1].setPoint(rect.getMinX(), rect.getMaxY()); // left top
            points[2].setPoint(rect.getMaxX(), rect.getMaxY()); // right top
            points[3].setPoint(rect.getMaxX(), rect.getMinY()); // right bottom
        }
    }

    Canvas2D::Canvas2D(QObject *parent, GLWidget *view)
        : Canvas(parent, view)
        , dragMode_(DRAG_NONE)
        , operateMode_(OperateMode::None)
        , view_(view)
    {
        connect(view, SIGNAL(signalDropFiles(QStringList,cocos2d::Vec2)), this, SLOT(onDropFiles(QStringList,cocos2d::Vec2)));
        connect(Framework::instance()->statusBar_, SIGNAL(signalSceneScaled(float, cocos2d::Point)), this, SLOT(onSceneScaled(float, cocos2d::Point)));

        NodePtr sceneRoot = Framework::instance()->getSceneRoot();

        bgLayer_ = cocos2d::LayerColor::create(cocos2d::Color4B::GRAY,
                                                canvasSize_.width,
                                                canvasSize_.height);
        sceneRoot->addChild(bgLayer_, -9999);

        cocos2d::Texture2D *texture = cocos2d::Director::getInstance()->getTextureCache()->addImage(Res::GridGray);
        if(texture != nullptr)
        {
            cocos2d::Texture2D::TexParams param;
            param.magFilter = GL_LINEAR;
            param.minFilter = GL_LINEAR;
            param.wrapS = GL_REPEAT;
            param.wrapT = GL_REPEAT;
            texture->setTexParameters(param);

            cocos2d::Size size = texture->getContentSizeInPixels();
            grid_ = cocos2d::Sprite::createWithTexture(texture, cocos2d::Rect(cocos2d::Vec2::ZERO, size * 70));
            sceneRoot->addChild(grid_, -9999);
        }

        ScenePtr scene = Framework::instance()->getScene();

        cocos2d::DrawNode *axis = cocos2d::DrawNode::create();
        axis->drawLine(cocos2d::Vec2(-1000, 0), cocos2d::Vec2(1000, 0), cocos2d::Color4F::RED);
        axis->drawLine(cocos2d::Vec2(0, -1000), cocos2d::Vec2(0, 1000), cocos2d::Color4F::GREEN);
        axis->setPosition(sceneRoot->getPosition());
        scene->addChild(axis, 9999);
        axisNode_ = axis;

        drawRect_ = cocos2d::DrawNode::create();
        scene->addChild(drawRect_, 9999);

        setGizmoMode(GizmoMode::Position);

        onTargetSet(Framework::instance()->getTargetNode());
    }

    Canvas2D::~Canvas2D()
    {
        if(bgLayer_)
        {
            bgLayer_->removeFromParent();
        }
        if(grid_)
        {
            grid_->removeFromParent();
        }
        if(axisNode_)
        {
            axisNode_->removeFromParent();
        }
        if(drawRect_)
        {
            drawRect_->removeFromParent();
        }
    }

    void Canvas2D::setGizmoMode(GizmoMode mode)
    {
        if(gizmoMode_ == mode)
        {
            return;
        }

        gizmoMode_ = mode;
    }

    void Canvas2D::setCanvasSize(cocos2d::Size size)
    {
        canvasSize_ = size;
        bgLayer_->setContentSize(canvasSize_);
    }

    void Canvas2D::onPropertyChange(PropertyParam &param)
    {
        if( param.name == "position" ||
            param.name == "scale" ||
            param.name == "rotation" ||
            param.name == "percentPosition" ||
            param.name == "percentSize" ||
            param.name == "anchor" ||
            param.name == "image" ||
            param.name == "text" ||
            param.name == "fntFile" ||
            param.name == "margin" ||
            param.name == "size")
        {
            drawSelectedRect();
        }
    }


    void Canvas2D::onMouseEvent(QMouseEvent *event)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            cocos2d::Point pt(event->x(), event->y());
            lastMousePosition_ = pt = cocos2d::Director::getInstance()->convertToUI(pt);

            if(event->button() == Qt::LeftButton)
            {
                if (!checkOperateMode(lastMousePosition_))
                {
                    doNodeSelect(lastMousePosition_);
                }

                if (targetNode_)
                {
                    lastSize_ = targetNode_->getContentSize();
                    lastPos_ = targetNode_->getPosition();
                }
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            operateMode_ = OperateMode::None;
        }
        else if(event->type() == QEvent::MouseMove)
        {
            cocos2d::Point pt(event->x(), event->y());
            pt = cocos2d::Director::getInstance()->convertToUI(pt);

            if(!almostEqual(pt, lastMousePosition_))
            {
                onMouseMove(event, pt);
                lastMousePosition_ = pt;
            }
        }
    }

    void Canvas2D::onKeyEvent(QKeyEvent *event)
    {
        if(event->type() == QEvent::KeyRelease)
        {
            if(event->key() == Qt::Key_Control)
            {
                drawSelectedRect();
            }
            else if(event->key() == Qt::Key_Delete)
            {
                if(targetNode_)
                {
                    Framework::instance()->pushCommand(new CmdDeleteNode(targetNode_));
                }
            }
        }
        else if(event->type() == QEvent::KeyPress)
        {
            handleDragEvent(event);
        }
    }

    bool Canvas2D::checkOperateMode(const cocos2d::Point &pt)
    {
        operateMode_ = OperateMode::None;
        dragMode_ = DragMode::DRAG_NONE;

        if(!targetNode_)
        {
            return false;
        }

        cocos2d::AffineTransform toLocal = targetNode_->getWorldToNodeAffineTransform();
        const cocos2d::Point localPt = cocos2d::PointApplyAffineTransform(pt, toLocal);
        const cocos2d::Rect dragRect = getNodeDragRect(targetNode_);

        // 尺寸会随着结点缩放而缩放，将世界空间下的尺寸转换到本地。
        cocos2d::Vec2 globalScale = getAffineTransformScale(toLocal);
        cocos2d::Size bounder(OUT_BOUNDER * globalScale.x, OUT_BOUNDER * globalScale.y);

        // 先判断是否是旋转操作，鼠标是否落在四个旋转区域上
        const int nPoints = 4;
        cocos2d::Point corners[nPoints];

        cocos2d::Rect rotateRect = dragRect;
        expandRect(rotateRect, bounder.width * 2.0f, bounder.height * 2.0f, bounder.width, bounder.height);
        rectToPoints(corners, rotateRect);

        static const int ROTATION_MODES[] = {
            DragMode::DRAG_BOTTOM | DragMode::DRAG_LEFT,
            DragMode::DRAG_TOP | DragMode::DRAG_LEFT,
            DragMode::DRAG_TOP | DragMode::DRAG_RIGHT,
            DragMode::DRAG_BOTTOM | DragMode::DRAG_RIGHT,
        };
        for (int i = 0; i < nPoints; i++)
        {
            cocos2d::Rect rt = cocos2d::Rect(corners[i], bounder);
            if (rt.containsPoint(localPt))
            {
                operateMode_ = OperateMode::Rotation;
                dragMode_ = ROTATION_MODES[i];
                return true;
            }
        }

        // 鼠标是否落在拉伸区域内
        cocos2d::Rect outerRect = dragRect;
        expandRect(outerRect, bounder.width, bounder.height);
        if(!outerRect.containsPoint(localPt))
        {
            return false;
        }

        dragMode_ = DragMode::DRAG_NONE;
        if(localPt.x <= 0)
        {
            dragMode_ |= DRAG_LEFT;
        }
        else if(localPt.x >= dragRect.size.width)
        {
            dragMode_ |= DRAG_RIGHT;
        }

        if(localPt.y <= 0)
        {
            dragMode_ |= DRAG_BOTTOM;
        }
        else if(localPt.y >= dragRect.size.height)
        {
            dragMode_ |= DRAG_TOP;
        }

        if(dragMode_ == DragMode::DRAG_NONE)
        {
            dragMode_ = DragMode::DRAG_CENTER;
            operateMode_ = OperateMode::Position;
        }
        else
        {
            operateMode_ = (gizmoMode_ == GizmoMode::Scale ? OperateMode::Scale : OperateMode::Size);
        }
        return true;
    }

    void Canvas2D::doNodeSelect(const cocos2d::Point & pt)
    {
        if(!rootNode_)
        {
            return;
        }

        NodePtr selected = findNodeByPoint(rootNode_, pt);
        if(selected != targetNode_)
        {
            Framework::instance()->hierarchy_->selectNode(selected);
        }
    }

    void Canvas2D::drawSelectedRect()
    {
        axisNode_->setPosition(Framework::instance()->getSceneRoot()->getPosition());
        drawRect_->clear();

        drawRect_->setVisible(bool(targetNode_));
        if(!targetNode_)
        {
            return;
        }

        cocos2d::AffineTransform toWorld = targetNode_->nodeToWorldTransform();
        // 将外围框尺寸转换到本地。因为随着节点的缩放，尺寸也在缩放
        cocos2d::Vec2 globalScale = getAffineTransformScale(toWorld);
        cocos2d::Point expand(OUT_BOUNDER, OUT_BOUNDER);
        expand.x /= globalScale.x;
        expand.y /= globalScale.y;

        // 绘制拖拽框
        const cocos2d::Rect dragRect = getNodeDragRect(targetNode_);
        drawRect(dragRect, toWorld, cocos2d::Color4F::RED);

        // 绘制4个角的拖拽框
        cocos2d::Rect cornerRect = dragRect;
        expandRect(cornerRect, expand.x, expand.y, 0, 0);
        drawRectVertex(cornerRect,  cocos2d::Size(expand.x, expand.y), toWorld, cocos2d::Color4F::BLUE);

        // 绘制拉伸框
        cocos2d::Rect rect = dragRect;
        expandRect(rect, expand.x, expand.y);
        drawRect(rect, toWorld, cocos2d::Color4F::BLUE);

        // 绘制4个旋转点
        expandRect(rect, expand.x, expand.y, 0, 0);
        drawRectVertex(rect, cocos2d::Size(expand.x, expand.y), toWorld, cocos2d::Color4F::BLUE);

        // 绘制锚点
        if(dragRect.size.width * globalScale.x > OUT_BOUNDER * 3.0f ||
           dragRect.size.height * globalScale.y > OUT_BOUNDER * 3.0f)
        {
            cocos2d::Point anchorPoint = targetNode_->getAnchorPointInPoints();
            rect.origin = anchorPoint;
            rect.size = cocos2d::Size::ZERO;
            expandRect(rect, expand.x * 0.5f, expand.y * 0.5f);
            drawRect(rect, toWorld, cocos2d::Color4F::GREEN);
        }
    }

    void Canvas2D::drawRect(const cocos2d::Rect &rect, const cocos2d::AffineTransform & world, const cocos2d::Color4F & color)
    {
        const int nPoints = 4;
        cocos2d::Point points[nPoints];
        rectToPoints(points, rect);

        cocos2d::AffineTransform toLocal = drawRect_->worldToNodeTransform();
        cocos2d::AffineTransform transform = cocos2d::AffineTransformConcat(world, toLocal);
        for(int i = 0; i < nPoints; ++i)
        {
            points[i] = cocos2d::PointApplyAffineTransform(points[i], transform);
        }

        for(int i = 0; i < nPoints; ++i)
        {
            int start = i % nPoints;
            int end = (i + 1) % nPoints;

            drawRect_->drawSegment(points[start], points[end], 1.0f, color);
        }
    }

    void Canvas2D::drawRectVertex(const cocos2d::Rect &rect, const cocos2d::Size &size, const cocos2d::AffineTransform & world, const cocos2d::Color4F & color)
    {
        cocos2d::Point points[4];
        rectToPoints(points, rect);

        cocos2d::Rect rc;
        for(int i = 0; i < 4; ++i)
        {
            rc.origin = points[i];
            rc.size = size;
            drawRect(rc, world, color);
        }
    }

    void Canvas2D::onNodeTouchMove(const cocos2d::Point & pt, const cocos2d::Point & old)
    {
        if(!targetNode_)
        {
            return;
        }

        cocos2d::AffineTransform t = targetNode_->getParent()->worldToNodeTransform();
        cocos2d::Point localPt = cocos2d::PointApplyAffineTransform(pt, t);
        cocos2d::Point localOld = cocos2d::PointApplyAffineTransform(old, t);

        cocos2d::Point delta = localPt - localOld;

        if (operateMode_ == OperateMode::Position)
        {
            doNodeDrag(delta);
        }
        else if (operateMode_ == OperateMode::Rotation)
        {
            doNodeRotate(pt, old);
        }
        else if (operateMode_ == OperateMode::Scale)
        {
            doNodeScale(pt, old);
        }
        else if (operateMode_ == OperateMode::Size)
        {
            doNodeResize(pt, old);
        }
    }

    void Canvas2D::doNodeDrag(const cocos2d::Point & delta)
    {
        cocos2d::Vec2 position = targetNode_->getPosition() + delta;
        cocos2d::Vec3 pos;
        pos.x = roundf(position.x);
        pos.y = roundf(position.y);
        pos.z = targetNode_->getPositionZ();

        JsonValue value = jsonFrom(pos);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "position", value));
    }

    void Canvas2D::doNodeResize(const cocos2d::Point & delta)
    {
        bool sizeChanged = false;
        cocos2d::Vec2 curAnchor = targetNode_->getAnchorPoint();
        cocos2d::Size newSize = targetNode_->getContentSize();
        cocos2d::Point posDelta(0, 0);

        if (dragMode_ & DRAG_RIGHT)
        {
            newSize.width = std::max(0.0f, roundf(newSize.width + delta.x));
            sizeChanged = true;
            posDelta.x = (newSize.width - lastSize_.width) * curAnchor.x;
        }
        else if (dragMode_ & DRAG_LEFT)
        {
            newSize.width = std::max(0.0f, roundf(newSize.width - delta.x));
            sizeChanged = true;
            posDelta.x = -(newSize.width - lastSize_.width) * (1 - curAnchor.x);
        }

        if (dragMode_ & DRAG_TOP)
        {
            newSize.height = std::max(0.0f, roundf(newSize.height + delta.y));
            sizeChanged = true;
            posDelta.y = (newSize.height - lastSize_.height) * curAnchor.y;
        }
        else if (dragMode_ & DRAG_BOTTOM)
        {
            newSize.height = std::max(0.0f, roundf(newSize.height - delta.y));
            sizeChanged = true;
            posDelta.y = -(newSize.height - lastSize_.height) * (1-curAnchor.y);
        }

        if (!sizeChanged)
        {
            return;
        }

        // 需要考虑到旋转因素。将移动量乘以旋转矩阵
        cocos2d::Vec3 pos3Delta(posDelta.x, posDelta.y, 0.0f);
        targetNode_->getNodeToParentTransform().transformVector(&pos3Delta);
        resetNodePos(cocos2d::Vec2(pos3Delta.x, pos3Delta.y));

        JsonValue value = jsonFrom(newSize);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "size", value));
    }

    void Canvas2D::doNodeResize(const cocos2d::Point & pt, const cocos2d::Point & last)
    {
        cocos2d::Point v1 = targetNode_->convertToNodeSpace(pt);
        cocos2d::Point v2 = targetNode_->convertToNodeSpace(last);
        cocos2d::Point delta = v1 - v2;
        doNodeResize(delta);
    }

    void Canvas2D::doNodeScale(const cocos2d::Point & pt, const cocos2d::Point & last)
    {
        cocos2d::Point pos = targetNode_->convertToNodeSpaceAR(last);
        cocos2d::Point point = targetNode_->convertToNodeSpaceAR(pt);

        float scaleX = targetNode_->getScaleX();
        float scaleY = targetNode_->getScaleY();
        if (dragMode_ & DRAG_RIGHT || dragMode_ & DRAG_LEFT)
        {
            scaleX = point.x / pos.x * scaleX;
        }

        if (dragMode_ & DRAG_TOP || dragMode_ & DRAG_BOTTOM)
        {
            scaleY = point.y / pos.y * scaleY;
        }

        JsonValue value = jsonFrom(cocos2d::Vec3(scaleX, scaleY, targetNode_->getScaleZ()));
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "scale", value));
    }

    void Canvas2D::doNodeRotate(const cocos2d::Point & pt, const cocos2d::Point & last)
    {
        cocos2d::Vec3 rotation = targetNode_->getRotation3D();
        cocos2d::Vec2 v1 = targetNode_->convertToNodeSpaceAR(pt);
        cocos2d::Vec2 v2 = targetNode_->convertToNodeSpaceAR(last);
        float sinAngle = v2.cross(v1) / (v1.getLength() * v2.getLength());
        float angle = asin(sinAngle) * (180 / MATH_PI);
        rotation = cocos2d::Vec3(rotation.x, rotation.y, rotation.z - angle);

        JsonValue value = jsonFrom(rotation);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "rotation", value));
    }

    void Canvas2D::resetNodePos(const cocos2d::Point & delta)
    {
        cocos2d::Point position = lastPos_ + delta;
//        position.x = roundf(position.x);
//        position.y = roundf(position.y);

        JsonValue value = jsonFrom(cocos2d::Vec3(position.x, position.y, targetNode_->getPositionZ()));
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "position", value));
    }

    bool Canvas2D::handleDragEvent(QKeyEvent *event)
    {
        if(!targetNode_)
        {
            return false;
        }

        cocos2d::Point delta;
        dragMode_ = DRAG_NONE;
        switch (event->key())
        {
            case Qt::Key_A:
                delta.x -= KeyboardMoveDelta;
                dragMode_ = DRAG_RIGHT;
                break;

            case Qt::Key_D:
                delta.x += KeyboardMoveDelta;
                dragMode_ = DRAG_RIGHT;
                break;

            case Qt::Key_W:
                delta.y += KeyboardMoveDelta;
                dragMode_ = DRAG_TOP;
                break;

            case Qt::Key_S:
                delta.y -= KeyboardMoveDelta;
                dragMode_ = DRAG_TOP;
                break;

            default:
                break;
        }

        if(dragMode_ != DRAG_NONE)
        {
            doNodeDrag(delta);
            return true;
        }
        return false;
    }

    void Canvas2D::onWheelEvent(QWheelEvent *event)
    {
        QPoint angleDelta = event->angleDelta() / 8;
        float delta = angleDelta.y() < 0.0f ? 0.9f : 1.1f;

        NodePtr root = Framework::instance()->getSceneRoot();

        cocos2d::Point pt(event->x(), event->y());
        pt = cocos2d::Director::getInstance()->convertToGL(pt);

        emit Framework::instance()->statusBar_->signalSceneScaled(root->getScale() * delta, pt);

        drawSelectedRect();
    }

    void Canvas2D::onSceneScaled(float scale, const cocos2d::Point&)
    {
        drawSelectedRect();
    }

    void Canvas2D::onMouseMove(QMouseEvent *event, const cocos2d::Point &pt)
    {
        if(event->buttons() & Qt::LeftButton)
        {
            onNodeTouchMove(pt, lastMousePosition_);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            dragScene(pt, lastMousePosition_);
        }
        else
        {
            checkOperateMode(pt);
            updateCursor();
        }
    }

    void Canvas2D::updateCursor()
    {
        if(operateMode_ == OperateMode::Rotation)
        {
            view_->setCursor(Qt::CrossCursor);
        }
        else if(dragMode_ == DRAG_CENTER)
        {
            view_->setCursor(Qt::SizeAllCursor);
        }
        // 左下角和右上角
        else if((dragMode_ & DRAG_LEFT && dragMode_ & DRAG_BOTTOM) ||
                (dragMode_ & DRAG_RIGHT && dragMode_ & DRAG_TOP))
        {
            view_->setCursor(Qt::SizeBDiagCursor);
        }
        // 左上角和右下角
        else if((dragMode_ & DRAG_LEFT && dragMode_ & DRAG_TOP) ||
                (dragMode_ & DRAG_RIGHT && dragMode_ & DRAG_BOTTOM))
        {
            view_->setCursor(Qt::SizeFDiagCursor);
        }
        else if(dragMode_ & (DRAG_LEFT | DRAG_RIGHT))
        {
            view_->setCursor(Qt::SizeHorCursor);
        }
        else if(dragMode_ & (DRAG_TOP | DRAG_BOTTOM))
        {
            view_->setCursor(Qt::SizeVerCursor);
        }
        else
        {
            view_->setCursor(Qt::ArrowCursor);
        }
    }

    void Canvas2D::dragScene(const cocos2d::Point &pt, const cocos2d::Point &last)
    {
        cocos2d::Point delta = pt - last;

        NodePtr root = Framework::instance()->getSceneRoot();
        root->setPosition(root->getPosition() + delta);

        drawSelectedRect();
    }

    void Canvas2D::onDropFiles(const QStringList &paths, const cocos2d::Vec2 &position)
    {
        Framework::instance()->pushCommand(new CmdDropFiles(paths, rootNode_, cocos2d::Vec3(position.x, position.y, 0.0f)));
    }

} // end namespace Editor
