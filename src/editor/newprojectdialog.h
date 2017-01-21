#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "config.h"
#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class EDITOR_DLL NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

private slots:
    void on_btnOK_clicked();

    void on_pushButtonProPath_clicked();

private:
    Ui::NewProjectDialog *ui;

    bool createFloder(const QString &projectPath);
};

#endif // NEWPROJECTDIALOG_H
