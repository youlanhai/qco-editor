#ifndef LAUNCHWINDOW_H
#define LAUNCHWINDOW_H

#include "config.h"
#include <QDialog>

namespace Ui {
class LaunchWindow;
}

class EDITOR_DLL LaunchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LaunchWindow(QWidget *parent = 0);
    ~LaunchWindow();

    void initListView();

private slots:

    void on_pushButtonNew_clicked();

    void on_pushButtonOK_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonSelect_clicked();

private:
    Ui::LaunchWindow *ui;

Q_SIGNALS:
    void clicked(const QModelIndex & index);

public slots:
    void onListItemClicked(const QModelIndex & index);
    void onListItemDoubleClicked(const QModelIndex & index);
};

#endif // LAUNCHWINDOW_H
