#include "user_configure.h"
#include "project_mgr.h"
#include "tools/json_tools.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <CCDirector.h>

IMPLEMENT_SINGLETON(Editor::UserConfigure);

namespace Editor
{
    UserConfigure::UserConfigure()
    {

    }

    bool UserConfigure::load(const QString &path)
    {
        filePath_ = path.toStdString();
        config_.setDict();

        mjson::Node config;
        if(!openJsonFile(filePath_, config) || !config.isDict())
        {
            return false;
        }

        const mjson::Dict &dict = config.refDict();
        for(const mjson::NodePair &pair : dict)
        {
            applyConfig(pair.key.asStdString(), pair.value);
        }

        config_ = config;
        return true;
    }

    bool UserConfigure::save()
    {
        Ui_MainWindow *ui = MainWindow::instance()->ui;
        mjson::Node &layout = config_["UILayout"];
        layout.setDict();
        layout["menuBarVisible"] = ui->menuBar->isVisible();
        layout["hierarchyVisible"] = ui->hierarchyWidget->isVisible();
        layout["resourceVisible"] = ui->resourceFolderWidget->isVisible();
        layout["inspectorVisible"] = ui->inspectorWidget->isVisible();

        layout["windowRect"] = jsonFrom(MainWindow::instance()->geometry());
        layout["hierarchyRect"] = jsonFrom(ui->hierarchyWidget->geometry());
        layout["resourceRect"] = jsonFrom(ui->resourceFolderWidget->geometry());
        layout["inspectorRect"] = jsonFrom(ui->inspectorWidget->geometry());

        if(!saveJsonFile(filePath_, config_))
        {
            return false;
        }
        return true;
    }

    bool UserConfigure::applyConfig(const std::string &key, const mjson::Node &value)
    {
        if(key == "showFPS")
            cocos2d::Director::getInstance()->setDisplayStats(json2bool(value));
        else if(key == "exportPath")
            ProjectManager::instance()->setExportPath(json2qstring(value));
        else if(key == "UILayout")
        {
            Ui_MainWindow *ui = MainWindow::instance()->ui;

            ui->menuBar->setVisible(json2bool(value["menuBarVisible"], true));
            ui->hierarchyWidget->setVisible(json2bool(value["hierarchyVisible"], true));
            ui->resourceFolderWidget->setVisible(json2bool(value["resourceVisible"], true));
            ui->inspectorWidget->setVisible(json2bool(value["inspectorVisible"], true));

            mjson::Node temp = value["windowRect"];
            if(temp.isArray())
                MainWindow::instance()->setGeometry(json2qrect(temp));

            temp = value["hierarchyRect"];
            if(temp.isArray())
                ui->hierarchyWidget->setGeometry(json2qrect(temp));

            temp = value["resourceRect"];
            if(temp.isArray())
                ui->resourceFolderWidget->setGeometry(json2qrect(temp));

            temp = value["inspectorRect"];
            if(temp.isArray())
                ui->inspectorWidget->setGeometry(json2qrect(temp));
        }
        else
            return false;

        return true;
    }

    void UserConfigure::setConfig(const std::string &key, const mjson::Node &value)
    {
        config_[key] = value;
    }
}
