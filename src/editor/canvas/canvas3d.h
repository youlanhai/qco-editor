#ifndef CANVAS3D_H
#define CANVAS3D_H

#include "canvas.h"
#include "runtime/smartpointer.h"

NS_CC_BEGIN
class Camera;
class DrawNode3D;
class Sprite3D;
NS_CC_END

namespace Editor
{
    class GizmoNode;
    class MouseEventDelegate;

    class EDITOR_DLL Canvas3D : public Canvas
    {
        Q_OBJECT
    public:
        enum MoveDirection
        {
            DIR_NONE    = 0,
            DIR_LEFT    = 1 << 0,
            DIR_FRONT   = 1 << 1,
            DIR_RIGHT   = 1 << 2,
            DIR_BACK    = 1 << 3
        };

        Canvas3D(QObject *parent, GLWidget *view);
        ~Canvas3D();

        void setGizmoMode(GizmoMode mode) override;

    public slots:
        virtual void onRootSet(cocos2d::Node *root) override;
        virtual void onTargetSet(cocos2d::Node *target) override;
        virtual void onPropertyChange(PropertyParam &param) override;
        virtual void onMouseEvent(QMouseEvent *event) override;
        virtual void onKeyEvent(QKeyEvent *event) override;
        virtual void onWheelEvent(QWheelEvent * event) override;
        virtual void onResize(float width, float height) override;
        virtual void onTick(float dt) override;

    private:
        virtual void drawSelectedRect() override;

        void doCameraRotation(const cocos2d::Vec2 &newPt, const cocos2d::Vec2 &oldPt);
        void doNodeSelection(const cocos2d::Vec2 &pt);
        void onNodePositionChange(const cocos2d::Vec3 &position);
        void onNodeRotationChange(const cocos2d::Vec3 &rotation);
        void onNodeScaleChange(const cocos2d::Vec3 &scale);

        SmartPointer<cocos2d::DrawNode3D>    drawNode_;
        SmartPointer<cocos2d::Camera>        camera_;

        SmartPointer<GizmoNode>              gizmo_;
        SmartPointer<cocos2d::Sprite3D>      ground_;

        cocos2d::Point      lastMousePosition_;

        float               cameraMoveSpeed_;
        int                 moveDirection_;

        MouseEventDelegate* mouseListener_;
    };

} // end namespace Editor

#endif // CANVAS3D_H
