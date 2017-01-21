#ifndef CANVASMGR_H
#define CANVASMGR_H

#include "../config.h"
#include <QObject>
#include <map>
#include "../cc_predefine.h"
#include "../common_types.h"

class QAction;
class GLWidget;
class PropertyParam;

namespace Ui {
class MainWindow;
}

namespace Editor
{
    class Canvas;
    class EDITOR_DLL CanvasMgr : public QObject
    {
        Q_OBJECT
    public:
        explicit CanvasMgr(QObject *parent, Ui::MainWindow *ui);

        void setCanvasMode(int mode);
        int getCanvasMode();

        void setGizmoMode(GizmoMode mode);

        Canvas* getCurrentCanvas(){ return currentCanvas_; }
    signals:

    public slots:
        void onRootSet(cocos2d::Node *root);
        void onTargetSet(cocos2d::Node *target);
        void onPropertyChange(PropertyParam &param);
        void onComponentPropertyChange(PropertyParam &param);

        void onActionTriggerCanvasMode(bool);


    private:
        int         canvasMode_;
        Canvas*     currentCanvas_;
        GLWidget*   glwidget_;

        std::map<QAction*, int> actions_;
    };

}

#endif // CANVASMGR_H
