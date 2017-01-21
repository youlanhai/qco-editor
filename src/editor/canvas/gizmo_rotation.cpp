#include "gizmo_rotation.h"
#include "../draw_node3d.h"
#include <2d/CCcamera.h>
#include <3d/CCPlane.h>

USING_NS_CC;

namespace Editor
{

    bool rayIntersectSphere(const Ray &ray, const Vec3 &origin, float radius, float *distance=nullptr)
    {
        Vec3 RO = origin - ray._origin;
        float ro2 = RO.lengthSquared();

        //射线起点在园内
        if(ro2 <= radius * radius)
        {
            if(distance != nullptr)
            {
                *distance = 0.0f;
            }
            return true;
        }

        float roProjection = ray._direction.dot(RO);

        float v = radius * radius + roProjection * roProjection - ro2;
        if(v < 0)
        {
            return false;
        }

        if(distance != nullptr)
        {
            *distance = roProjection - sqrtf(v);
        }
        return true;
    }

    Axis getMinAxis(const Vec3 &p)
    {
        if(fabs(p.x) < fabs(p.y))
        {
            return fabs(p.x) < fabs(p.z) ? Axis::X : Axis::Z;
        }
        else
        {
            return fabs(p.y) < fabs(p.z) ? Axis::Y : Axis::Z;
        }
    }

    Vec3 getUnitVectorByAxis(Axis i)
    {
        Vec3 v = Vec3::ZERO;
        ((float*)&v)[int(i)] = 1.0f;
        return v;
    }

    GizmoRotation::GizmoRotation()
        : rotationAxis_(Axis::None)
        , sphereRadius_(0.0f)
    {

    }

    bool GizmoRotation::init()
    {
        if(!GizmoNode::init())
        {
            return false;
        }

        DrawNode3D *node = DrawNode3D::create();
        node->drawCycle(Vec3::ZERO, Vec3::UNIT_Y, Vec3::UNIT_Z, Color4B::RED);
        node->drawCycle(Vec3::ZERO, Vec3::UNIT_X, Vec3::UNIT_Z, Color4B::GREEN);
        node->drawCycle(Vec3::ZERO, Vec3::UNIT_X, Vec3::UNIT_Y, Color4B::BLUE);
        node->setDepthTestEnable(false);
        addChild(node);

        root_ = node;
        return true;
    }

    bool GizmoRotation::onMousePress(const cocos2d::Vec2 &pt)
    {
        Ray ray = screenPtToWorldRay(pt, camera_);

        // calculate the radius of sphere.
        Vec3 origin, point;
        Mat4 transform = root_->getNodeToWorldTransform();
        transform.transformPoint(Vec3::ZERO, &origin);
        transform.transformPoint(Vec3::UNIT_X, &point);

        sphereRadius_ = origin.distance(point);

        // get the intersect position
        float distance = 0.0f;
        if(!rayIntersectSphere(ray, origin, sphereRadius_, &distance))
        {
            return false;
        }
        crossPosition_ = ray._origin + ray._direction * distance;

        // find the neast axis.
        Vec3 localCrossPosition;
        Mat4 inverseTransform = transform;
        inverseTransform.inverse();
        inverseTransform.transformPoint(crossPosition_, &localCrossPosition);

        rotationAxis_ = getMinAxis(localCrossPosition /*- Vec3::ZERO*/);

        // calculate the tangent
        Vec3 radiusVector = crossPosition_ - origin;
        Vec3 unitAxis = getUnitVectorByAxis(rotationAxis_);
        Vec3::cross(unitAxis, radiusVector, &crossTangent_);
        crossTangent_.normalize();

        oldRotation_ = target_->getRotation3D();
        return true;
    }

    void GizmoRotation::onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 & /*last*/)
    {
        // calculate the position of mouse in world.
        // 拖拽点所在的平面是经过交点且与相机近平面平行的面。
        Vec3 look;
        camera_->getNodeToWorldTransform().getForwardVector(&look);
        cocos2d::Plane dragPlane_(look, crossPosition_);
        Ray mouseRay = screenPtToWorldRay(pt, camera_);
        Vec3 point = mouseRay.intersects(dragPlane_);

        float projectionLength = crossTangent_.dot(point - crossPosition_);

        float angle = projectionLength / sphereRadius_ * 90.0f;
        Vec3 rotation = oldRotation_;
        if(rotationAxis_ == Axis::X)
        {
            rotation.x += angle;
        }
        else if(rotationAxis_ == Axis::Y)
        {
            rotation.y += angle;
        }
        else if(rotationAxis_ == Axis::Z)
        {
            rotation.z -= angle;
        }
        //target_->setRotation3D(rotation);

        if(rotationChangeListener_)
        {
            rotationChangeListener_(rotation);
        }
    }

    void GizmoRotation::onMouseRelease(const cocos2d::Vec2 &pt)
    {
        rotationAxis_ = Axis::None;
    }

}
