#include "newfiledialog.h"
#include "framework.h"
#include "resource_mgr.h"
#include "tools/qstring_tool.h"
#include "runtime/json_helper.h"

#include <QMessageBox>

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);
    this->register_radio_id();
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

void NewFileDialog::register_radio_id()
{
    qbg = new QButtonGroup();
    qbg->addButton(ui->radioButton2DScene);
    qbg->addButton(ui->radioButton3DScene);

    qbg->setId(ui->radioButton2DScene, (int)Editor::SceneType::TP_2D);
    qbg->setId(ui->radioButton3DScene, (int)Editor::SceneType::TP_3D);
}

void NewFileDialog::on_btnOK_clicked()
{
    QString name = ui->textFileName->text();
    if(name.isEmpty())
    {
        QMessageBox::critical(nullptr, tr("Error"), QString(tr("Please input a file name.")));
        return;
    }

    int id = qbg->checkedId();
    if(id == -1)
    {
        QMessageBox::critical(nullptr, tr("Error"), QString(tr("Please select file type.")));
        return;
    }

    if(Editor::getFileExt(name) != ".layout")
    {
        name += ".layout";
    }

    if(Editor::Framework::instance()->resource_->existFile(name))
    {
        QMessageBox::critical(nullptr, tr("Error"), QString(tr("The file name has been exist.")));
        return;
    }

    Editor::Framework::instance()->newLayout((Editor::SceneType)id);

    QString fullPath = Editor::Framework::instance()->resource_->toAbsolutePath(name);
    if(!Editor::Framework::instance()->saveLayout(fullPath))
    {
        // force to write a file.
        JsonValue value;
        value.setDict();
        saveJsonFile(fullPath.toStdString(), value);
    }
    Editor::Framework::instance()->resource_->addFile(name);

    accept();
}

void NewFileDialog::on_btnCancal_clicked()
{
    reject();
}
