#ifndef __Editor__EditorCanvas__
#define __Editor__EditorCanvas__

#include "../config.h"
#include <QObject>
#include <json/document.h>

#include <platform/CCPlatformMacros.h>
#include <base/ccTypes.h>
#include "runtime/smartpointer.h"

#include "../common_types.h"

NS_CC_BEGIN
class Node;
class DrawNode;
struct AffineTransform;
NS_CC_END

class PropertyParam;

class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

class GLWidget;

namespace Editor
{

    class EDITOR_DLL Canvas : public QObject
    {
        Q_OBJECT
    public:
        
        enum DragMode
        {
            DRAG_NONE   = 0,
            DRAG_LEFT   = 1 << 0,
            DRAG_RIGHT  = 1 << 1,
            DRAG_TOP    = 1 << 2,
            DRAG_BOTTOM = 1 << 3,
            DRAG_CENTER = 1 << 4,
        };
        
        Canvas(QObject *parent, GLWidget *view);
        ~Canvas();
        
        void togglePreview();
        void setBackGroundColor(const cocos2d::Color4B & color);
        virtual void setGizmoMode(GizmoMode mode);

    public slots:
        virtual void onRootSet(cocos2d::Node *root);
        virtual void onTargetSet(cocos2d::Node *target);
        virtual void onPropertyChange(PropertyParam &param);
        virtual void onComponentPropertyChange(PropertyParam &param);
        virtual void onMouseEvent(QMouseEvent *event);
        virtual void onKeyEvent(QKeyEvent *event);
        virtual void onWheelEvent(QWheelEvent * event);
        virtual void onResize(float width, float height);

        virtual void onTick(float dt){}

        virtual void setCanvasSize(cocos2d::Size size){}
        virtual cocos2d::Size getCanvasSize();
        virtual void onDropFiles(const QStringList &paths, const cocos2d::Vec2 &position);

    protected:

        virtual void drawSelectedRect();
        
        SmartPointer<cocos2d::Node>      rootNode_;
        SmartPointer<cocos2d::Node>      targetNode_;

        cocos2d::Point                      lastMousePosition_;
        GizmoMode                           gizmoMode_;

        cocos2d::Size       canvasSize_;
    };
}

#endif /* defined(__Editor__EditorCanvas__) */
