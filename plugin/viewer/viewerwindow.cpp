#include "viewerwindow.h"
#include "ui_viewerwindow.h"

#include "editor/plugin_mgr.h"
#include "editor/plugin_interface.h"
#include "editor/framework.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewerWindow)
{
    ui->setupUi(this);

    QStringList titles;
    titles << tr("Name") << tr("Path");
    ui->treeWidget->setHeaderLabels(titles);

    const Editor::PluginMgr::Plugins plugins = Editor::Framework::instance()->plugin_->getPlugins();
    foreach(const Editor::PluginInterface *plugin, plugins)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)nullptr);
        item->setText(0, plugin->getName());
        item->setText(1, plugin->getPath());
        ui->treeWidget->addTopLevelItem(item);
    }

    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
}

ViewerWindow::~ViewerWindow()
{
    delete ui;
}
