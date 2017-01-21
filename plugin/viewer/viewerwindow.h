#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QDialog>

namespace Ui {
    class ViewerWindow;
}

class ViewerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ViewerWindow(QWidget *parent = 0);
    ~ViewerWindow();

private:
    Ui::ViewerWindow *ui;
};

#endif // VIEWERWINDOW_H
