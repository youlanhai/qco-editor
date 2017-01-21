#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include "editor/framework.h"
#include "editor/project_mgr.h"
#include "tools/file_tools.h"
#include "tools/qstring_tool.h"

#include <QFileDialog>
#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

bool NewProjectDialog::createFloder(const QString &projectPath)
{
    if (QFileInfo::exists(projectPath))
    {
        QMessageBox::about(this,tr("Info"),tr("The project has been exist, please select another path."));
        return false;
    }

    QDir proDir;
    if (!proDir.mkdir(projectPath))
    {
        QMessageBox::about(this,tr("Info"),tr("Create project folder failed."));
        return false;
    }

    return true;
}

void NewProjectDialog::on_btnOK_clicked()
{
    QString proName = ui->lineEditProName->text();
    QString proPath = ui->lineEditProPath->text();
    if (proName.isEmpty() || proPath.isEmpty())
    {
        QMessageBox::about(this, tr("Error"), tr("Invalid path."));
        return;
    }

    QString strProPath = Editor::joinPath(proPath, proName);
    if(!createFloder(strProPath))
    {
        return;
    }

    Editor::ProjectManager::instance()->newProject(strProPath, proName);
    this->accept();
}

void NewProjectDialog::on_pushButtonProPath_clicked()
{
    QString defaultPath = QString::fromStdString(Editor::getAppModulePath());
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select folder"), defaultPath);
    if(!fileName.isEmpty())
    {
        ui->lineEditProPath->setText(fileName);
    }
}
