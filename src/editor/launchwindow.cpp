#include "launchwindow.h"
#include "ui_launchwindow.h"

#include "framework.h"
#include "project_mgr.h"
#include "newprojectdialog.h"

#include "runtime/json_helper.h"
#include "tools/file_tools.h"
#include "tools/json_tools.h"
#include "tools/log_tool.h"
#include "tools/json_tools.h"

#include <QFileDialog>
#include <QStringListModel>
#include <QMessagebox.h>

LaunchWindow::LaunchWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LaunchWindow)
{
    ui->setupUi(this);
    this->initListView();
}

LaunchWindow::~LaunchWindow()
{
    delete ui;
}

void LaunchWindow::initListView()
{
    const QString &historyFile = Editor::ProjectManager::instance()->getHistoryFilePath();
    if (! QFileInfo::exists(historyFile))
    {
        return;
    }

    QStringList strList;

    JsonValue document;
    if (!openJsonFile(historyFile.toStdString(), document) || !document.isArray())
    {
        return;
    }

    for(const JsonValue &value : document.refArray())
    {
        strList.push_front(json2qstring(value));
    }
    QStringListModel * model = new QStringListModel(this);
    model->setStringList(strList);
    ui->listView->setModel(model);

    connect(ui->listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onListItemClicked(const QModelIndex &)));
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onListItemDoubleClicked(QModelIndex)));
}

void LaunchWindow::on_pushButtonNew_clicked()
{
    NewProjectDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        this->accept();
    }
}

void LaunchWindow::on_pushButtonOK_clicked()
{
    QString fileName = ui->lineEdit->text();
    if (fileName.isEmpty())
    {
        QMessageBox::critical(this,tr("Error"),tr("Please select project path."));
        return;
    }

    if (! QFileInfo::exists(fileName))
    {
        QMessageBox::critical(this,tr("Error"),tr("The project path doesn't exist."));
        return;
    }

    Editor::ProjectManager::instance()->open(fileName);

    this->accept();
}

void LaunchWindow::on_pushButtonCancel_clicked()
{
    this->reject();
}

void LaunchWindow::on_pushButtonSelect_clicked()
{
    QString defaultPath = QString::fromUtf8(Editor::getAppModulePath().c_str());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Project"), defaultPath, tr("Project Files (*.project)"));
    if(!fileName.isEmpty())
    {
        ui->lineEdit->setText(fileName);
    }
}

void LaunchWindow::onListItemClicked(const QModelIndex & index)
{
    QStringListModel * model = (QStringListModel *)ui->listView->model();
    QStringList strList = model->stringList();
    QString str = strList[index.row()];
    ui->lineEdit->setText(str);
}

void LaunchWindow::onListItemDoubleClicked(const QModelIndex & index)
{
    onListItemClicked(index);
    on_pushButtonOK_clicked();
}
