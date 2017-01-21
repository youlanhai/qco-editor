#include "canvas_mgr.h"
#include "canvas.h"
#include "canvas_creator.h"
#include "ui_mainwindow.h"
#include "../framework.h"

#include "../properties/property_param.h"

#include <2d/CCNode.h>
#include <2d/CCComponent.h>

namespace Editor
{
    CanvasMgr::CanvasMgr(QObject *parent, Ui::MainWindow *ui)
        : QObject(parent)
        , canvasMode_(-1)
        , currentCanvas_(nullptr)
        , glwidget_(ui->cocos_widget)
    {
        actions_[ui->actionCanvas3D] = (int)SceneType::TP_3D;
        actions_[ui->actionCanvas2D] = (int)SceneType::TP_2D;
        actions_[ui->actionCanvasTile] = (int)SceneType::TP_TILE;

        for(auto &pair : actions_)
        {
            connect(pair.first, SIGNAL(triggered(bool)), this, SLOT(onActionTriggerCanvasMode(bool)));
        }
    }

    void CanvasMgr::setCanvasMode(int mode)
    {
        if(canvasMode_ == mode)
        {
            return;
        }

        for(auto &pair : actions_)
        {
            pair.first->setChecked(pair.second == mode);
        }

        canvasMode_ = mode;

        Framework::instance()->lockGL();

        if(currentCanvas_ != nullptr)
        {
            delete currentCanvas_;
        }
        currentCanvas_ = CanvasFactory::instance()->createCanvas((SceneType)mode, this, glwidget_);
        currentCanvas_->onRootSet(Framework::instance()->getRootNode());
        currentCanvas_->onTargetSet(Framework::instance()->getTargetNode());
    }

    void CanvasMgr::setGizmoMode(GizmoMode mode)
    {
        currentCanvas_->setGizmoMode(mode);
    }

    void CanvasMgr::onRootSet(cocos2d::Node *root)
    {
        currentCanvas_->onRootSet(root);
    }

    void CanvasMgr::onTargetSet(cocos2d::Node *target)
    {
        currentCanvas_->onTargetSet(target);
    }

    void CanvasMgr::onPropertyChange(PropertyParam &param)
    {
        if(Framework::instance()->getTargetNode() == param.object)
        {
            currentCanvas_->onPropertyChange(param);
        }
    }

    void CanvasMgr::onComponentPropertyChange(PropertyParam &param)
    {
        if(((cocos2d::Component*)param.object)->getOwner() == Framework::instance()->getTargetNode())
        {
            currentCanvas_->onComponentPropertyChange(param);
        }
    }

    void CanvasMgr::onActionTriggerCanvasMode(bool)
    {
        QAction *p = dynamic_cast<QAction*>(sender());
        assert(p != nullptr);

        int mode = actions_[p];
        setCanvasMode(mode);
    }
}
