#ifndef GIZMOPOSITION_H
#define GIZMOPOSITION_H

#include "gizmo.h"

namespace Editor
{

    class EDITOR_DLL GizmoPosition : public GizmoNode
    {
    public:
        typedef std::function<void(const cocos2d::Vec3 &)> PositionChangeListener;

        CREATE_FUNC(GizmoPosition)

        GizmoPosition();

        bool init() override;

        bool onMousePress(const cocos2d::Vec2 &pt) override;
        void onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 &last) override;
        void onMouseRelease(const cocos2d::Vec2 &pt) override;

        void setPositionChangedListener(PositionChangeListener listener){ positionListener_ = listener; }

    private:
        cocos2d::Node*  intersectNode_;

        // the position is in world coordinate system.
        cocos2d::Vec3   srcOrigin_;
        cocos2d::Vec3   intersectAxis_;
        cocos2d::Vec3   intersectOrigin_;
        cocos2d::Plane  verticalPlane_;

        PositionChangeListener positionListener_;
    };

}

#endif // GIZMOPOSITION_H
