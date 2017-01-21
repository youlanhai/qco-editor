#ifndef EDITORGIZMO_H
#define EDITORGIZMO_H

#include "../config.h"
#include <2d/CCNode.h>
#include <3d/CCPlane.h>
#include <3d/CCRay.h>

#include <functional>

namespace Editor
{
    cocos2d::Color3B getAxisColorByName(const std::string &name);
    cocos2d::Ray screenPtToWorldRay(const cocos2d::Vec2 &pt, cocos2d::Camera *camera);

    class EDITOR_DLL MouseEventDelegate
    {
    public:
        virtual ~MouseEventDelegate(){}

        virtual bool onMousePress(const cocos2d::Vec2 &pt) = 0;
        virtual void onMouseDrag(const cocos2d::Vec2 &pt, const cocos2d::Vec2 &last){}
        virtual void onMouseRelease(const cocos2d::Vec2 &pt){}
    };

    class EDITOR_DLL GizmoNode : public cocos2d::Node, public MouseEventDelegate
    {
    public:
        CREATE_FUNC(GizmoNode)

        GizmoNode();
        ~GizmoNode();

        virtual bool init() override;
        virtual void update(float dt) override;

        virtual bool onMousePress(const cocos2d::Vec2 &pt) override;

        void setTarget(cocos2d::Node *target);
        void setGlobalPosition(const cocos2d::Vec3 &position);
        void setCamera(cocos2d::Camera *camera);

    protected:
        cocos2d::Camera*    camera_;
        cocos2d::Node*      target_;
        cocos2d::Node*      root_;
    };

}

#endif // EDITORGIZMO_H
