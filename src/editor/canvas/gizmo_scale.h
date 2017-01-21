#ifndef EDITORGIZMOSCALE_H
#define EDITORGIZMOSCALE_H

#include "gizmo.h"

namespace Editor
{
    class EDITOR_DLL GizmoScale : public GizmoNode
    {
    public:
        typedef std::function<void(const cocos2d::Vec3 &)> ScaleChangeListener;

        CREATE_FUNC(GizmoScale)

        GizmoScale();

        bool init() override;

        bool onMousePress(const cocos2d::Vec2 &pt) override;
        void onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 &last) override;
        void onMouseRelease(const cocos2d::Vec2 &pt) override;

        void setScaleChangedListener(ScaleChangeListener listener){ scaleListener_ = listener; }

    private:
        cocos2d::Node * intersectNode_;

        cocos2d::Vec3 srcOrigin_;   //触摸开始时记录下node的当前缩放比例
        cocos2d::Vec3 intersectAxis_;
        cocos2d::Vec3   intersectOrigin_;
        cocos2d::Plane  verticalPlane_;

        ScaleChangeListener scaleListener_;
    };
}

#endif // EDITORGIZMOSCALE_H
