#ifndef GIZMOROTATION_H
#define GIZMOROTATION_H

#include "gizmo.h"

namespace Editor
{
    enum class Axis
    {
        None = - 1,
        X = 0,
        Y = 1,
        Z = 2,
    };

    class EDITOR_DLL GizmoRotation : public GizmoNode
    {
    public:
        typedef std::function<void(const cocos2d::Vec3 &)> RotationChangeListener;

        CREATE_FUNC(GizmoRotation)
        GizmoRotation();

        bool init() override;

        bool onMousePress(const cocos2d::Vec2 &pt) override;
        void onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 &last) override;
        void onMouseRelease(const cocos2d::Vec2 &pt) override;

        void setRotationChangeListener(RotationChangeListener listener){ rotationChangeListener_ = listener; }

    private:
        Axis        rotationAxis_;
        float       sphereRadius_;

        cocos2d::Vec3   crossPosition_; //鼠标与球体的相交点
        cocos2d::Vec3   crossTangent_; //相交点处的切线
        cocos2d::Vec3   oldRotation_;

        RotationChangeListener rotationChangeListener_;
    };

}

#endif // GIZMOROTATION_H
