#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include <QMainWindow>
#include <QTimer>

#include "cc_predefine.h"

class AppDelegate;
namespace Ui {
class MainWindow;
}

class GLWidget;
class QLabel;

class EDITOR_DLL MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    GLWidget* getGLWidget();

    static MainWindow* instance(){ return s_instance; }

    void doSaveEvent();


    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *);
    void checkOperateAction(QAction *action);
    void showOperationResult(bool ok, const QString &operation);

    QList<QAction*>     operateActions_;
    QLabel*             labMousePosition_;
    QTimer              timerShowPosition_;

    static MainWindow*  s_instance;
private slots:
    void onTargetSet(cocos2d::Node *target);
    void onTimerShowPosition();
    void on_actionFileOpen_triggered();
    void on_actionFileSave_triggered();
    void on_actionFileClear_triggered();
    void on_actionOperateView_triggered();
    void on_actionOperateMove_triggered();
    void on_actionOperateRotate_triggered();
    void on_actionOperateScale_triggered();
    void on_actionFileNew_triggered();
    void on_actionFileSaveAs_triggered();
    void on_actionProjectOpen_triggered();
    void on_actionProjectNew_triggered();
    void on_actionToolsStartSimulator_triggered();
    void on_actionToolsSetting_triggered();
    void on_actionProjectSave_triggered();
    void on_actionFileExport_triggered();
    void on_actionConvertPathToUUID_triggered();
    void on_actionConvertUUIDToPath_triggered();
    void on_actionConvertAllFilesPathToUUID_triggered();
    void on_actionConvertAllFilesUUIDToPath_triggered();
    void on_actionEditUndo_triggered();
    void on_actionEditRedo_triggered();
    void on_actionAlignLeft_triggered();
    void on_actionAlignTop_triggered();
    void on_actionAlignRight_triggered();
    void on_actionAlignBottom_triggered();
    void on_actionAlignHorizontalCenter_triggered();
    void on_actionAlignVerticalCenter_triggered();
    void on_actionSameWidth_triggered();
    void on_actionSameHeight_triggered();
    void on_actionSameHorizontalSpace_triggered();
    void on_actionSameVerticeSpace_triggered();
    void on_actionSameSize_triggered();
};

#endif // MAINWINDOW_H
