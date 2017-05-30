#ifndef CANVAS2D_H
#define CANVAS2D_H

#include "canvas.h"

NS_CC_BEGIN
class LayerColor;
class Sprite;
NS_CC_END

namespace Editor
{
    enum class OperateMode
    {
        None,
        Position,
        Rotation,
        Scale,
        Size
    };
    class CanvasOperator;

    class EDITOR_DLL Canvas2D : public Canvas
    {
        Q_OBJECT
    public:
        Canvas2D(QObject *parent, GLWidget *view);
        ~Canvas2D();

        void setGizmoMode(GizmoMode mode) override;

    public slots:
        virtual void onPropertyChange(PropertyParam &param) override;
        virtual void onMouseEvent(QMouseEvent *event) override;
        virtual void onKeyEvent(QKeyEvent *event) override;
        virtual void onWheelEvent(QWheelEvent *event) override;
        virtual void onDropFiles(const QStringList &paths, const cocos2d::Vec2 &position) override;
        virtual void onSceneScaled(float scale, const cocos2d::Point &);

    private:
        virtual void drawSelectedRect() override;

        void drawRect(const cocos2d::Rect &rect, const cocos2d::AffineTransform & world, const cocos2d::Color4F & color);
        void drawRectVertex(const cocos2d::Rect &rect, const cocos2d::Size &size, const cocos2d::AffineTransform & world, const cocos2d::Color4F & color);

        void onNodeTouchMove(const cocos2d::Point & pt, const cocos2d::Point & old);

        void doNodeSelect(const cocos2d::Point & pt);
        void doNodeDrag(const cocos2d::Point & delta);
        void doNodeResize(const cocos2d::Point & delta);
        void doNodeScale(const cocos2d::Point & pt, const cocos2d::Point & last);
        void doNodeRotate(const cocos2d::Point & pt, const cocos2d::Point & last);
        void doNodeResize(const cocos2d::Point & pt, const cocos2d::Point & last);
        void resetNodePos(const cocos2d::Point & delta);

        bool handleDragEvent(QKeyEvent *event);
        bool checkOperateMode(const cocos2d::Point &point);

        void onMouseMove(QMouseEvent *event, const cocos2d::Point &pt);
        void updateCursor();
        void dragScene(const cocos2d::Point &pt, const cocos2d::Point &last);

        SmartPointer<cocos2d::DrawNode>  drawRect_;

        int                 dragMode_;
        OperateMode         operateMode_;
        cocos2d::Size       lastSize_;   //last鼠标上一次按下时
        cocos2d::Vec2       lastPos_;    //last鼠标上一次按下时

    protected:
        virtual void setCanvasSize(cocos2d::Size size) override;

        GLWidget*           view_;
        SmartPointer<cocos2d::LayerColor> bgLayer_;
        SmartPointer<cocos2d::Sprite>    grid_;
        SmartPointer<cocos2d::Node>      axisNode_;
    };

} //end namespace Editor

#endif // CANVAS2D_H
