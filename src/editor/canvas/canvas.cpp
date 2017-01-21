#include "canvas.h"
#include "../glwidget.h"

#include <2d/CCNode.h>

namespace Editor
{

    Canvas::Canvas(QObject *parent, GLWidget *view)
        : QObject(parent)
        , gizmoMode_(GizmoMode::None)
        , canvasSize_(cocos2d::Size(480.0f, 640.0f))
    {
        connect(view, SIGNAL(signalMouseEvent(QMouseEvent*)), this, SLOT(onMouseEvent(QMouseEvent*)));
        connect(view, SIGNAL(signalKeyEvent(QKeyEvent*)), this, SLOT(onKeyEvent(QKeyEvent*)));
        connect(view, SIGNAL(signalWheelEvent(QWheelEvent*)), this, SLOT(onWheelEvent(QWheelEvent*)));
        connect(view, SIGNAL(signalResize(float,float)), this, SLOT(onResize(float,float)));
        connect(view, SIGNAL(signalTick(float)), this, SLOT(onTick(float)));
    }
    
    Canvas::~Canvas()
    {
    }

    void Canvas::onRootSet(cocos2d::Node *root)
    {
        rootNode_ = root;
    }

    void Canvas::onTargetSet(cocos2d::Node *target)
    {
        targetNode_ = target;
        drawSelectedRect();
    }

    void Canvas::onPropertyChange(PropertyParam &param)
    {

    }

    void Canvas::onComponentPropertyChange(PropertyParam &param)
    {

    }

    void Canvas::onMouseEvent(QMouseEvent *event)
    {

    }

    void Canvas::onKeyEvent(QKeyEvent *event)
    {

    }

    void Canvas::onWheelEvent(QWheelEvent * event)
    {

    }

    void Canvas::onResize(float width, float height)
    {

    }

    void Canvas::drawSelectedRect()
    {

    }

    void Canvas::togglePreview()
    {

    }
    
    void Canvas::setBackGroundColor(const cocos2d::Color4B & color)
    {

    }

    void Canvas::setGizmoMode(GizmoMode mode)
    {

    }

    cocos2d::Size Canvas::getCanvasSize()
    {
        return canvasSize_;
    }

    void Canvas::onDropFiles(const QStringList & /*paths*/, const cocos2d::Vec2 & /*position*/)
    {

    }
} // end namespace Editor
