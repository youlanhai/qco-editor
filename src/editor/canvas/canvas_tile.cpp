#include "canvas_tile.h"
#include "editor/framework.h"
#include "editor/resource.h"
#include "editor/document.h"
#include "editor/status_bar.h"
#include "editor/tools/math_tool.h"
#include "editor/properties/property_param.h"

#include "runtime/extend/tile_collision_node.h"
#include "tile_painter.h"

#include <CCDirector.h>

#include <QMouseEvent>
#include <QKeyEvent>

namespace Editor
{

    Canvas* CanvasTile::create(QObject *parent, GLWidget *view)
    {
        return new CanvasTile(parent, view);
    }

    CanvasTile::CanvasTile(QObject *parent, GLWidget *view)
        : Canvas(parent, view)
        , painter_(nullptr)
    {
    }

    CanvasTile::~CanvasTile()
    {
        if(painter_)
        {
            delete painter_;
        }
    }

    void CanvasTile::onTargetSet(cocos2d::Node *target)
    {
        Canvas::onTargetSet(target);

        pip::TileCollisionNode *node = nullptr;
        if(target)
        {
            node = dynamic_cast<pip::TileCollisionNode*>(target);
        }

        if(tileCollision_ == node)
        {
            return;
        }

        if(painter_)
        {
            delete painter_;
            painter_ = nullptr;
        }

        tileCollision_ = node;
        if(tileCollision_)
        {
            JsonHandle config = Framework::instance()->document_->findNodeConfig(tileCollision_);
            painter_ = createPainter(tileCollision_, config);
        }
    }

    void CanvasTile::onPropertyChange(PropertyParam &param)
    {
        if(!tileCollision_)
        {
            return;
        }

        if(param.name == "mapType")
        {
            if(painter_)
            {
                delete painter_;
            }
            painter_ = createPainter(tileCollision_, param.properties);
        }

        if(painter_)
        {
            painter_->onPropertyChange(param);
        }
    }

    void CanvasTile::onComponentPropertyChange(PropertyParam &param)
    {

    }

    void CanvasTile::onMouseEvent(QMouseEvent *event)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            cocos2d::Point pt(event->x(), event->y());
            lastMousePosition_ = pt = cocos2d::Director::getInstance()->convertToUI(pt);

            if(event->button() == Qt::LeftButton)
            {
                if(painter_)
                {
                    painter_->paintGrid(pt);
                }
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            if(tileCollision_)
            {
                std::string buffer;
                pip::TileCollisionNode::packCollisionData(buffer, tileCollision_->getDatas());

                Framework::instance()->emitNodePropertyChange(tileCollision_, "datas", jsonFrom(buffer));
            }
        }
        else if(event->type() == QEvent::MouseMove)
        {
            cocos2d::Point pt(event->x(), event->y());
            pt = cocos2d::Director::getInstance()->convertToUI(pt);

            if(!almostEqual(pt, lastMousePosition_))
            {
                if(event->buttons() & Qt::LeftButton)
                {
                    if(painter_)
                    {
                        painter_->paintGrid(pt);
                    }
                }
                else if(event->buttons() & Qt::RightButton)
                {
                    dragScene(pt, lastMousePosition_);
                }
                lastMousePosition_ = pt;
            }
        }
    }

    void CanvasTile::onKeyEvent(QKeyEvent *event)
    {

    }

    void CanvasTile::onWheelEvent(QWheelEvent *event)
    {
        QPoint angleDelta = event->angleDelta() / 8;
        float delta = angleDelta.y() < 0.0f ? 0.9f : 1.1f;

        NodePtr root = Framework::instance()->getSceneRoot();

        cocos2d::Point pt(event->x(), event->y());
        pt = cocos2d::Director::getInstance()->convertToGL(pt);

        emit Framework::instance()->statusBar_->signalSceneScaled(root->getScale() * delta, pt);
    }

    void CanvasTile::dragScene(const cocos2d::Point &pt, const cocos2d::Point &last)
    {
        cocos2d::Point delta = pt - last;

        NodePtr root = Framework::instance()->getSceneRoot();
        root->setPosition(root->getPosition() + delta);
    }

    TilePainter* CanvasTile::createPainter(pip::TileCollisionNode *node, const JsonHandle &config)
    {
        TilePainter *p = new TilePainter();
        p->setComponent(node, config);
        return p;
    }
}
