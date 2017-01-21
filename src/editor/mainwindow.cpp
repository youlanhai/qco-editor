#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "framework.h"
#include "newfiledialog.h"
#include "newprojectdialog.h"
#include "editor_setting.h"
#include "document.h"
#include "project_mgr.h"
#include "resource_mgr.h"
#include "tools/file_tools.h"
#include "tools/log_tool.h"
#include "canvas/editor_multi_selector.h"
#include "importer.h"
#include "canvas/canvas_mgr.h"
#include "user_configure.h"

#include <base/CCDirector.h>
#include <2d/CCScene.h>

#include <sstream>

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLabel>
#include <QSplitter>

/*static*/ MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_instance = this;

    ui->setupUi(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, ui->resourceFolderWidget);
    splitter->addWidget(ui->resourceTree);
    splitter->addWidget(ui->resourceContentTable);
    splitter->setOpaqueResize(false);
    ui->resourceFolderWidget->setWidget(splitter);
    QList<int> sizes = {150, 500};
    splitter->setSizes(sizes);

    operateActions_.push_back(ui->actionOperateView);
    operateActions_.push_back(ui->actionOperateMove);
    operateActions_.push_back(ui->actionOperateRotate);
    operateActions_.push_back(ui->actionOperateScale);
    checkOperateAction(ui->actionOperateMove);

    labMousePosition_ = new QLabel();
    ui->statusBar->addPermanentWidget(labMousePosition_);
    timerShowPosition_.setInterval(100);
    timerShowPosition_.start();
    connect(&timerShowPosition_, SIGNAL(timeout()), this, SLOT(onTimerShowPosition()));

    ui->menuView->addAction(ui->hierarchyWidget->toggleViewAction());
    ui->menuView->addAction(ui->inspectorWidget->toggleViewAction());
    ui->menuView->addAction(ui->resourceFolderWidget->toggleViewAction());
    ui->menuView->addAction(ui->logContentWidget->toggleViewAction());
    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->mainToolBar->toggleViewAction());
    ui->menuView->addAction(ui->alignToolBar->toggleViewAction());

    ui->logContentWidget->setVisible(false);

    //ui->mainToolBar->setStyleSheet("spacing:0px");
    //ui->alignToolBar->setStyleSheet("spacing:0px");

    Editor::Framework::initInstance();
}

MainWindow::~MainWindow()
{
    Editor::Framework::finiInstance();
    delete ui;

    if(s_instance == this)
    {
        s_instance = nullptr;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        event->ignore();
        return;
    }

    Editor::UserConfigure::instance()->save();

    LOG_DEBUG("Editor will close.");

    timerShowPosition_.stop();

    ui->cocos_widget->makeCurrent();
    Editor::Framework::instance()->closeLayout();
    cocos2d::Director::getInstance()->end();
    cocos2d::Director::getInstance()->mainLoop();
    ui->cocos_widget->doneCurrent();

    event->accept();
}

GLWidget* MainWindow::getGLWidget()
{
    return ui->cocos_widget;
}

void MainWindow::doSaveEvent()
{
    on_actionFileSave_triggered();
}

void MainWindow::onTimerShowPosition()
{
    QVector2D mousePos = ui->cocos_widget->getMousePosition();

    std::ostringstream ss;
    ss << "view(" << mousePos.x() << ", " << mousePos.y() << ") ";

    Editor::ScenePtr scene = Editor::Framework::instance()->getScene();
    if(scene)
    {
        cocos2d::Vec2 pos = cocos2d::Director::getInstance()->convertToUI(cocos2d::Vec2(mousePos.x(), mousePos.y()));
        pos = scene->convertToNodeSpace(pos);
        ss << "scene(" << std::round(pos.x) << ", " << std::round(pos.y) << ")";
    }

    Editor::NodePtr node = Editor::Framework::instance()->getTargetNode();
    if(node)
    {
        cocos2d::Vec2 pos = cocos2d::Director::getInstance()->convertToUI(cocos2d::Vec2(mousePos.x(), mousePos.y()));
        pos = node->convertToNodeSpace(pos);
        ss << "local(" << std::round(pos.x) << ", " << std::round(pos.y) << ")";
    }

    labMousePosition_->setText(QString::fromStdString(ss.str()));
}

void MainWindow::onTargetSet(cocos2d::Node *target)
{
    bool isMultiMode = target == Editor::Framework::instance()->document_->getMultiSelector();
    ui->menuAlignment->setEnabled(isMultiMode);
    ui->alignToolBar->setEnabled(isMultiMode);
}

void MainWindow::on_actionFileOpen_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    QString defaultPath = Editor::Framework::instance()->getResourcePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), defaultPath, "Editor Files (*.layout)");
    if(!fileName.isEmpty())
    {
        EDITOR_LOCK_GL_SCOPE();
        Editor::Framework::instance()->loadLayout(fileName.toUtf8().data());
    }
}

void MainWindow::on_actionFileSave_triggered()
{
    QString name = Editor::Framework::instance()->document_->getFileName();
    if(!name.isEmpty())
    {
        EDITOR_LOCK_GL_SCOPE();
        Editor::Framework::instance()->saveLayout(name);
    }
    else
    {
        on_actionFileSaveAs_triggered();
    }
}

void MainWindow::on_actionFileClear_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }
    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->closeLayout();
}

void MainWindow::on_actionOperateView_triggered()
{
    checkOperateAction(ui->actionOperateView);

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->canvasMgr_->setGizmoMode(Editor::GizmoMode::None);
}

void MainWindow::on_actionOperateMove_triggered()
{
    checkOperateAction(ui->actionOperateMove);

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->canvasMgr_->setGizmoMode(Editor::GizmoMode::Position);
}

void MainWindow::on_actionOperateRotate_triggered()
{
    checkOperateAction(ui->actionOperateRotate);

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->canvasMgr_->setGizmoMode(Editor::GizmoMode::Rotation);
}

void MainWindow::on_actionOperateScale_triggered()
{
    checkOperateAction(ui->actionOperateScale);

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->canvasMgr_->setGizmoMode(Editor::GizmoMode::Scale);
}

void MainWindow::checkOperateAction(QAction *action)
{
    for(QAction *act : operateActions_)
    {
        act->setChecked(action == act);
    }
}

void MainWindow::on_actionFileNew_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    NewFileDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionFileSaveAs_triggered()
{
    QString resourcePath = Editor::Framework::instance()->getResourcePath();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), resourcePath, "Editor Files (*.layout)");
    if(!fileName.isEmpty())
    {
        if(!fileName.startsWith(resourcePath))
        {
            QString info = QString(tr("The destination path must under '%1'")).arg(resourcePath);
            QMessageBox::critical(nullptr, tr("Error"), info);
            return;
        }

        if(Editor::Framework::instance()->saveLayout(fileName))
        {
            QString relativePath = Editor::Framework::instance()->resource_->toResourcePath(fileName);
            Editor::Framework::instance()->resource_->addFile(relativePath);
        }
    }
}

void MainWindow::on_actionProjectOpen_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    QString defaultPath = Editor::Framework::instance()->getResourcePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Project"), defaultPath, "Editor Files (*.project)");
    if(!fileName.isEmpty())
    {
        EDITOR_LOCK_GL_SCOPE();
        Editor::Framework::instance()->closeLayout();
        Editor::ProjectManager::instance()->open(fileName);
        QString resPath = Editor::ProjectManager::instance()->getResourcePath();
        Editor::Framework::instance()->setResourcePath(resPath);
        Editor::Framework::instance()->newLayout(Editor::SceneType::TP_2D);
    }
}

void MainWindow::on_actionProjectNew_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    NewProjectDialog dialog;
    if(QDialog::Accepted == dialog.exec())
    {
        Editor::Framework::instance()->closeLayout();
        QString resPath = Editor::ProjectManager::instance()->getResourcePath();
        Editor::Framework::instance()->setResourcePath(resPath);
        Editor::Framework::instance()->newLayout(Editor::SceneType::TP_2D);
    }
}

void MainWindow::on_actionToolsStartSimulator_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    if(!Editor::Framework::instance()->checkCurrentLayoutExist())
    {
        return;
    }

    QString layoutFile = Editor::Framework::instance()->document_->getFileName();

    std::string path = Editor::getAppModulePath() + "editor_simulator.app";
    QString qpath = QString::fromStdString(path);
    if(!QFileInfo::exists(qpath))
    {
        LOG_ERROR("simulator app '%s' was not found.", path.c_str());
        return;
    }

    QString resourcePath = Editor::Framework::instance()->getResourcePath();

    QString cmd = QString("open \"%1\" --args \"%2\" \"%3\"").arg(qpath, layoutFile, resourcePath);
    QProcess::startDetached(cmd);
}

void MainWindow::on_actionToolsSetting_triggered()
{
    EditorSetting editorSetting;
    editorSetting.exec();
}

void MainWindow::on_actionProjectSave_triggered()
{
    Editor::ProjectManager::instance()->save();
}

void MainWindow::on_actionFileExport_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    bool ret = Editor::Framework::instance()->importer_->exportAssets();
    showOperationResult(ret, "Export");
}

void MainWindow::on_actionConvertPathToUUID_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    if(!Editor::Framework::instance()->checkCurrentLayoutExist())
    {
        return;
    }

    QString layoutFile = Editor::Framework::instance()->document_->getFileName();
    if(!Editor::Framework::instance()->importer_->convertPathToUUID(layoutFile))
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Convert failed."));
        return;
    }

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->reloadCurrentLayout();
}

void MainWindow::on_actionConvertUUIDToPath_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    if(!Editor::Framework::instance()->checkCurrentLayoutExist())
    {
        return;
    }

    QString layoutFile = Editor::Framework::instance()->document_->getFileName();
    if(!Editor::Framework::instance()->importer_->convertUUIDToPath(layoutFile))
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Convert failed."));
        return;
    }

    EDITOR_LOCK_GL_SCOPE();
    Editor::Framework::instance()->reloadCurrentLayout();
}

void MainWindow::on_actionConvertAllFilesPathToUUID_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    bool ret = Editor::Framework::instance()->importer_->convertPathToUUID("");
    showOperationResult(ret, "Convert");
}

void MainWindow::on_actionConvertAllFilesUUIDToPath_triggered()
{
    if(!Editor::Framework::instance()->checkCurrentLayoutModified())
    {
        return;
    }

    bool ret = Editor::Framework::instance()->importer_->convertUUIDToPath("");
    showOperationResult(ret, "Convert");
}

void MainWindow::showOperationResult(bool ok, const QString &operation)
{
    if(!ok)
    {
        QMessageBox::critical(nullptr, tr("Error"), operation + " failed.");
    }
    else
    {
        QMessageBox::information(nullptr, tr("Information"), operation + " succed.");
    }
}

void MainWindow::on_actionEditUndo_triggered()
{
    if(Editor::Framework::instance()->document_ != nullptr)
    {
        EDITOR_LOCK_GL_SCOPE();
        Editor::Framework::instance()->document_->getUndoStack().undo();
    }
}

void MainWindow::on_actionEditRedo_triggered()
{
    if(Editor::Framework::instance()->document_ != nullptr)
    {
        EDITOR_LOCK_GL_SCOPE();
        Editor::Framework::instance()->document_->getUndoStack().redo();
    }
}

void MainWindow::on_actionAlignLeft_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignLeft();
    }
}

void MainWindow::on_actionAlignTop_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignTop();
    }
}

void MainWindow::on_actionAlignRight_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignRight();
    }
}

void MainWindow::on_actionAlignBottom_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignBottom();
    }
}

void MainWindow::on_actionAlignHorizontalCenter_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignHCenter();
    }
}

void MainWindow::on_actionAlignVerticalCenter_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setAlignVCenter();
    }
}

void MainWindow::on_actionSameWidth_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setSameWidth();
    }
}

void MainWindow::on_actionSameHeight_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setSameHeight();
    }
}

void MainWindow::on_actionSameHorizontalSpace_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setSameHorizontalGap();
    }
}

void MainWindow::on_actionSameVerticeSpace_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setSameVerticalGap();
    }
}

void MainWindow::on_actionSameSize_triggered()
{
    auto selector = Editor::Framework::instance()->document_->getMultiSelector();
    if(selector)
    {
        EDITOR_LOCK_GL_SCOPE();
        selector->setSameSize();
    }
}
