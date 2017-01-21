#include "plugin_viewer.h"

#include "ui_mainwindow.h"
#include "editor/mainwindow.h"

#include "viewerwindow.h"

DEFINE_PLUGIN(viewer, Editor::PluginViewer)

namespace Editor
{

    PluginViewer::PluginViewer()
        : actionToggleView_(nullptr)
        , view_(nullptr)
    {

    }

    PluginViewer::~PluginViewer()
    {
        actionToggleView_ = nullptr;
        if(view_ != nullptr)
        {
            delete view_;
        }
    }

    bool PluginViewer::init()
    {
        PluginInterface::init();

        QMenu *menu = MainWindow::instance()->ui->menuView;
        actionToggleView_ = menu->addAction(tr("Plugin Viewer"), this, SLOT(onActionToggleView()));
        actionToggleView_->setCheckable(true);
        actionToggleView_->setChecked(false);
        return true;
    }

    void PluginViewer::onActionToggleView()
    {
        if(nullptr == view_)
        {
            view_ = new ViewerWindow();
            view_->setAttribute(Qt::WA_DeleteOnClose, true);
            connect(view_, SIGNAL(destroyed(QObject*)), this, SLOT(onViewDestroy()));
        }
        view_->show();
        actionToggleView_->setChecked(true);
    }

    void PluginViewer::onViewDestroy()
    {
        view_ = nullptr;
        if(actionToggleView_)
        {
            actionToggleView_->setChecked(false);
        }
    }
}
