#include "canvas3d.h"

#include "../draw_node3d.h"
#include "../framework.h"
#include "../tools/mesh_tools.h"
#include "../command/cmd_property_change.h"
#include "../properties/property_param.h"
#include "../resource.h"

#include "runtime/object_loader.h"

#include "gizmo_position.h"
#include "gizmo_rotation.h"
#include "gizmo_scale.h"

#include <base/CCDirector.h>
#include <2d/CCcamera.h>
#include <2d/CClight.h>
#include <3d/CCsprite3d.h>
#include <physics3d/CCPhysics3D.h>

#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>

unsigned short ViewCameraMask = (unsigned short)cocos2d::CameraFlag::USER8;

USING_NS_CC;

namespace Editor
{

    Canvas3D::Canvas3D(QObject *parent, GLWidget *view)
        : Canvas(parent, view)
        , cameraMoveSpeed_(500.0f)
        , moveDirection_(DIR_NONE)
        , mouseListener_(nullptr)
    {
        auto scene = Framework::instance()->getScene();

        auto director = Director::getInstance();

        float aspect = director->getWinSize().width / director->getWinSize().height;
        camera_ = Camera::createPerspective(60, aspect, 1.0f, 10000.0f);
        camera_->setCameraFlag(CameraFlag::USER8);
        camera_->setPosition3D(Vec3(0.0f, 500.0f, 500.0f));
        camera_->lookAt(Vec3::ZERO);
        scene->addChild(camera_);

        scene->getPhysics3DWorld()->setDebugDrawEnable(true);
        scene->setPhysics3DDebugCamera(camera_);

        drawNode_ = DrawNode3D::create();
        drawNode_->setCameraMask(ViewCameraMask);
        scene->addChild(drawNode_);

        ground_ = createSquareModel(10000, 10000, Res::GridGreen, 100, Res::GridShader);
        ground_->setCameraMask(ViewCameraMask);
        ground_->setLightMask(0); // don't use lighting and default shader.
        ground_->setOpacity(254); // force transparent.
        scene->addChild(ground_);

        scene->getDefaultCamera()->setVisible(false);

        setGizmoMode(GizmoMode::Position);
    }

    Canvas3D::~Canvas3D()
    {
        auto scene = Framework::instance()->getScene();
        if(scene)
        {
            scene->getDefaultCamera()->setVisible(true);

            scene->getPhysics3DWorld()->setDebugDrawEnable(false);
            scene->setPhysics3DDebugCamera(nullptr);
        }
        if(camera_)
        {
            camera_->removeFromParent();
        }
        if(drawNode_)
        {
            drawNode_->removeFromParent();
        }
        if(ground_)
        {
            ground_->removeFromParent();
        }
        if(gizmo_)
        {
            gizmo_->removeFromParent();
        }
    }

    void Canvas3D::setGizmoMode(GizmoMode mode)
    {
        if(gizmoMode_ == mode)
        {
            return;
        }

        if(gizmo_)
        {
            if(mouseListener_ == gizmo_)
            {
                mouseListener_ = nullptr;
            }
            gizmo_->removeFromParent();
            gizmo_ = nullptr;
        }

        gizmoMode_ = mode;

        if(mode == GizmoMode::Position)
        {
            GizmoPosition *gizmo = GizmoPosition::create();
            gizmo->setPositionChangedListener(std::bind(&Canvas3D::onNodePositionChange, this, std::placeholders::_1));
            gizmo_ = gizmo;
        }
        else if(mode == GizmoMode::Rotation)
        {
            GizmoRotation *gizmo = GizmoRotation::create();
            gizmo->setRotationChangeListener(std::bind(&Canvas3D::onNodeRotationChange, this, std::placeholders::_1));
            gizmo_ = gizmo;
        }
        else if(mode == GizmoMode::Scale)
        {
            GizmoScale * gizmo = GizmoScale::create();
            gizmo->setScaleChangedListener(std::bind(&Canvas3D::onNodeScaleChange, this, std::placeholders::_1));
            gizmo_ = gizmo;
            if(targetNode_ != nullptr)
            {
                gizmo_->setRotation3D(targetNode_->getRotation3D());
            }
        }
        else
        {
            gizmo_ = GizmoNode::create();
        }

        gizmo_->setCamera(camera_);
        gizmo_->setCameraMask(ViewCameraMask);
        gizmo_->setVisible(targetNode_ != nullptr);
        gizmo_->setTarget(targetNode_);
        Framework::instance()->getScene()->addChild(gizmo_);
    }

    void Canvas3D::onRootSet(cocos2d::Node *root)
    {
        Canvas::onRootSet(root);
    }

    void Canvas3D::onTargetSet(cocos2d::Node *target)
    {
        targetNode_ = target;

        gizmo_->setVisible(target != nullptr);
        gizmo_->setTarget(target);

        drawSelectedRect();
    }

    void Canvas3D::onPropertyChange(PropertyParam &param)
    {
        if(param.name == "position" || param.name == "scale" ||
                param.name == "rotation")
        {
            drawSelectedRect();
        }
    }

    void Canvas3D::onMouseEvent(QMouseEvent *event)
    {
        cocos2d::Point pt(event->x(), event->y());
        pt = cocos2d::Director::getInstance()->convertToUI(pt);

        if(event->type() == QEvent::MouseButtonPress)
        {
            if(event->button() == Qt::LeftButton)
            {
                if(gizmo_->isVisible() && gizmo_->onMousePress(pt))
                {
                    mouseListener_ = gizmo_;
                }
                else
                {
                    doNodeSelection(pt);
                }
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            if(event->button() == Qt::LeftButton)
            {
                if(mouseListener_ != nullptr)
                {
                    mouseListener_->onMouseRelease(pt);
                    mouseListener_ = nullptr;
                }
            }
        }
        else if(event->type() == QEvent::MouseMove)
        {
            if(event->buttons() & Qt::LeftButton)
            {
                if(mouseListener_ != nullptr)
                {
                    mouseListener_->onMouseDrag(pt, lastMousePosition_);
                }
            }
            else if(event->buttons() & Qt::RightButton)
            {
                doCameraRotation(pt, lastMousePosition_);
            }
        }

        lastMousePosition_ = pt;
    }

    void Canvas3D::onWheelEvent(QWheelEvent * event)
    {
        QPoint angleDelta = event->angleDelta() / 8;

        float delta = angleDelta.y() / 180.0f * cameraMoveSpeed_;

        Vec3 direction;
        camera_->getNodeToParentTransform().getForwardVector(&direction);

        Vec3 position = camera_->getPosition3D() + direction * delta;
        camera_->setPosition3D(position);
    }

    void Canvas3D::onKeyEvent(QKeyEvent *event)
    {
        int dir = 0;
        switch(event->key())
        {
        case Qt::Key_Left:
        case Qt::Key_A:
            dir = DIR_LEFT;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            dir = DIR_RIGHT;
            break;
        case Qt::Key_Up:
        case Qt::Key_W:
            dir = DIR_FRONT;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            dir = DIR_BACK;
            break;
        default:
            break;
        }

        if(dir != DIR_NONE)
        {
            if(event->type() == QEvent::KeyPress)
            {
                moveDirection_ |= dir;
            }
            else if(event->type() == QEvent::KeyRelease)
            {
                moveDirection_ &= ~dir;
            }
        }

        if(event->type() == QEvent::KeyRelease)
        {
            if(event->key() == Qt::Key_Delete)
            {
                if(targetNode_)
                {
                    Framework::instance()->removeNode(targetNode_);
                }
            }
        }
    }

    void Canvas3D::onResize(float width, float height)
    {
        float aspect = width / height;
        camera_->initPerspective(60, aspect, 1.0f, 10000.0f);
    }

    void Canvas3D::onTick(float dt)
    {
        if(moveDirection_ != DIR_NONE)
        {
            const Mat4 & transform = camera_->getNodeToParentTransform();
            Vec3 direction;
            if(moveDirection_ & DIR_LEFT)
            {
                transform.getLeftVector(&direction);
            }
            else if(moveDirection_ & DIR_RIGHT)
            {
                transform.getRightVector(&direction);
            }

            if(moveDirection_ & DIR_FRONT)
            {
                transform.getForwardVector(&direction);
                direction.y = 0.0f;
            }
            else if(moveDirection_ & DIR_BACK)
            {
                transform.getBackVector(&direction);
                direction.y = 0.0f;
            }

            direction.normalize();

            Vec3 position = camera_->getPosition3D() + direction * (dt * cameraMoveSpeed_);
            camera_->setPosition3D(position);
        }
    }

    void Canvas3D::drawSelectedRect()
    {
        drawNode_->clear();

        if(targetNode_)
        {
            Sprite3D *sprite = dynamic_cast<Sprite3D*>(targetNode_.get());
            if(nullptr != sprite)
            {
                drawNode_->drawAABB(sprite->getAABB(), Color4B::WHITE);
            }
        }
    }

    void Canvas3D::doCameraRotation(const cocos2d::Vec2 &newPt, const cocos2d::Vec2 &oldPt)
    {
        Vec2 delta = newPt - oldPt;

        Size winSize = Director::getInstance()->getWinSize();

        float dx = delta.x / winSize.width;
        float dy = delta.y / winSize.height;

        Vec3 rotation = camera_->getRotation3D();
        rotation.y -= dx * 180.0f;
        rotation.x += dy * 180.0f;
        camera_->setRotation3D(rotation);
    }

    void Canvas3D::doNodeSelection(const cocos2d::Vec2 &pt)
    {
        cocos2d::Node *node = nullptr;

        cocos2d::Ray ray = screenPtToWorldRay(pt, camera_);
        Physics3DWorld::HitResult result;
        Vec3 rayEnd = ray._origin + ray._direction * camera_->getFarPlane();
        if(Framework::instance()->getScene()->getPhysics3DWorld()->rayCast(ray._origin, rayEnd, &result))
        {
            node = (cocos2d::Node*)result.hitObj->getUserData();
        }

        Framework::instance()->setTargetNode(node);
    }

    void Canvas3D::onNodePositionChange(const cocos2d::Vec3 &position)
    {
        gizmo_->setGlobalPosition(position);

        Vec3 localPosition;
        targetNode_->getParent()->getWorldToNodeTransform().transformPoint(position, &localPosition);
        targetNode_->setPosition3D(localPosition);

        JsonValue value = jsonFrom(localPosition);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "position", value));
    }

    void Canvas3D::onNodeRotationChange(const cocos2d::Vec3 &rotation)
    {
        targetNode_->setRotation3D(rotation);

        JsonValue value = jsonFrom(rotation);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "rotation", value));
    }

    void Canvas3D::onNodeScaleChange(const cocos2d::Vec3 &scale)
    {
        targetNode_->setScaleX(scale.x);
        targetNode_->setScaleY(scale.y);
        targetNode_->setScaleZ(scale.z);

        JsonValue value = jsonFrom(scale);
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, "scale", value));
    }
}
