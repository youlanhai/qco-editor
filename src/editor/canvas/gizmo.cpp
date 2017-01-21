#include "gizmo.h"
#include <2d/CCcamera.h>

USING_NS_CC;

namespace Editor
{

    Color3B getAxisColorByName(const std::string &name)
    {
        if(name == "x") return Color3B::RED;
        if(name == "y") return Color3B::GREEN;
        if(name == "z") return Color3B::BLUE;
        return Color3B::WHITE;
    }

    Ray screenPtToWorldRay(const Vec2 &pt, Camera *camera)
    {
        Vec3 rayOrg = Vec3::ZERO;
        camera->getNodeToWorldTransform().transformPoint(&rayOrg);

        Vec3 pn = camera->unprojectGL(Vec3(pt.x, pt.y, -1.0f));
        Vec3 rayDir = pn - rayOrg;

        return Ray(rayOrg, rayDir);
    }

    GizmoNode::GizmoNode()
        : root_(nullptr)
        , target_(nullptr)
        , camera_(nullptr)
    {

    }

    GizmoNode::~GizmoNode()
    {

    }

    bool GizmoNode::init()
    {
        if(!Node::init())
        {
            return false;
        }

        scheduleUpdate();
        return true;
    }

    void GizmoNode::setTarget(cocos2d::Node *target)
    {
        target_ = target;
        if(target_)
        {
            Vec3 position;
            target_->getNodeToWorldTransform().transformPoint(Vec3::ZERO, &position);
            setGlobalPosition(position);
        }
    }

    void GizmoNode::setGlobalPosition(const cocos2d::Vec3 &position)
    {
        Vec3 temp = position;
        if(_parent != nullptr)
        {
            _parent->getWorldToNodeTransform().transformPoint(&temp);
        }
        setPosition3D(temp);
    }

    void GizmoNode::setCamera(cocos2d::Camera *camera)
    {
        camera_ = camera;
    }

    bool GizmoNode::onMousePress(const cocos2d::Vec2 & /*pt*/)
    {
        return false;
    }

    void GizmoNode::update(float dt)
    {
        Node::update(dt);

        if(root_ != nullptr)
        {
            Vec3 cameraPosition = Vec3::ZERO;
            camera_->getNodeToWorldTransform().transformPoint(&cameraPosition);

            Vec3 rootPosition = Vec3::ZERO;
            root_->getNodeToWorldTransform().transformPoint(&rootPosition);

            float distance = cameraPosition.distance(rootPosition);
            float scale = distance / 5.0f;
            root_->setScale(scale);
        }
    }
}

