#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include "config.h"
#include <QDialog>
#include "ui_newfiledialog.h"

namespace Ui {
class NewFileDialog;
}

class EDITOR_DLL NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();

private slots:
    void on_btnOK_clicked();
    void on_btnCancal_clicked();

private:
    Ui::NewFileDialog *ui;
    QButtonGroup * qbg;

    void register_radio_id();
};

#endif // NEWFILEDIALOG_H
