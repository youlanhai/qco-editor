#include "gizmo_scale.h"

#include <3d/CCsprite3d.h>
#include <3d/CCRay.h>
#include <2d/CCcamera.h>

#include "../tools/mesh_tools.h"

USING_NS_CC;

#define ONCE_SCALE_FOR_PIXEL 100    //每移动100像素缩放1倍

namespace  Editor
{
    GizmoScale::GizmoScale()
        :   intersectNode_(nullptr)
    {

    }

    bool GizmoScale::init()
    {
        if (! GizmoNode::init())
        {
            return false;
        }

        root_ = Node::create();
        this->addChild(root_);

        float radius = 0.05f;
        float length = 10.0f;

        Sprite3D * sprite;

        //  x
        sprite = createCube(Vec3(length * radius, radius, radius), Color4B::RED);
        sprite->setName("x");
        sprite->setPosition3D(Vec3((length +  1) * radius, radius, 0.0f));
        sprite->setForce2DQueue(true);
        root_->addChild(sprite);

        //  y
        sprite = createCube(Vec3(radius, length * radius, radius), Color4B::GREEN);
        sprite->setName("y");
        sprite->setPosition3D(Vec3(0.0f, (length + 2) * radius, 0.0f));
        sprite->setForce2DQueue(true);
        root_->addChild(sprite);

        //  z
        sprite = createCube(Vec3(radius, radius, length * radius), Color4B::BLUE);
        sprite->setName("z");
        sprite->setPosition3D(Vec3(0.0f, radius, (length + 1) * radius));
        sprite->setForce2DQueue(true);
        root_->addChild(sprite);

        return true;
    }

    bool GizmoScale::onMousePress(const Vec2 &pt)
    {
        if(!target_)
        {
            return false;
        }

        Ray ray = screenPtToWorldRay(pt, camera_);
        float minDistance = camera_->getFarPlane();

        intersectNode_ = nullptr;

        // get the best intersection node.
        for(Node *child : root_->getChildren())
        {
            Sprite3D *sprite = dynamic_cast<Sprite3D*>(child);

            AABB aabb = sprite->getAABBRecursively();
            float distance;
            if(ray.intersects(aabb, &distance) && distance < minDistance)
            {
                intersectNode_ = sprite;
                minDistance = distance;
            }
        }

        if(intersectNode_ != nullptr)
        {
            float scaleX = target_->getScaleX();
            float scaleY = target_->getScaleY();
            float scaleZ = target_->getScaleZ();
            srcOrigin_ = cocos2d::Vec3(scaleX, scaleY, scaleZ);

            intersectOrigin_ = ray._origin + ray._direction * minDistance;

            // get intersection axis.
            if(intersectNode_->getName() == "x")
            {
                intersectAxis_ = Vec3::UNIT_X;
            }
            else if(intersectNode_->getName() == "y")
            {
                intersectAxis_ = Vec3::UNIT_Y;
            }
            else if(intersectNode_->getName() == "z")
            {
                intersectAxis_ = Vec3::UNIT_Z;
            }

            // convert the axis to world coordinate system.
//            root_->getNodeToWorldTransform().transformVector(&intersectAxis_);
            intersectAxis_.normalize();

            Vec3 planeNormal;
            Vec3::cross(intersectAxis_, ray._direction, &planeNormal);

            //求出与相交平面垂直的平面
            Vec3::cross(intersectAxis_, planeNormal, &planeNormal);
            verticalPlane_.initPlane(planeNormal, intersectOrigin_);

            intersectNode_->setColor(Color3B::YELLOW);
        }

        return intersectNode_ != nullptr;
    }

    void GizmoScale::onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 &last)
    {
        if(intersectNode_ == nullptr || target_ == nullptr)
        {
           return;
        }

        Ray ray = screenPtToWorldRay(pt, camera_);

        float distance = ray.dist(verticalPlane_);

        if (distance != 0.0f)
        {
            Vec3 intersectPosition = ray._origin + ray._direction * distance;

            float projection = intersectAxis_.dot(intersectPosition - intersectOrigin_);
            Vec3 dstPosition = intersectAxis_ * projection;
            Vec3 disScale = dstPosition/ONCE_SCALE_FOR_PIXEL;
            Vec3 scale = srcOrigin_ + disScale;
            if (scaleListener_)
            {
                scaleListener_(scale);
            }
        }
    }

    void GizmoScale::onMouseRelease(const cocos2d::Vec2 &pt)
    {
        if(intersectNode_)
        {
            intersectNode_->setColor(getAxisColorByName(intersectNode_->getName()));
        }

        intersectNode_ = nullptr;
    }
}

