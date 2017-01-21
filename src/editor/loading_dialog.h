#ifndef LOADING_DIALOG_H
#define LOADING_DIALOG_H

#include "config.h"
#include <QDialog>
#include <QTimer>
#include <functional>

namespace Ui {
    class LoadingDialog;
}

class EDITOR_DLL LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    typedef std::function<bool(LoadingDialog *)> StepFunc;

    explicit LoadingDialog(StepFunc fun, QWidget *parent = 0);
    ~LoadingDialog();

    void setProgress(float progress);
    void showMessage(const QString &msg);

    void cancelLoading();
    void finishLoading();

signals:
    void signalStep(LoadingDialog *dialog);

private slots:
    void onTimer();
    void on_btnCancel_clicked();

private:
    Ui::LoadingDialog *ui;
    StepFunc    stepFunc_;
    bool        isCanceled_;
    bool        isFinished_;
    QTimer      timer_;
    QString     logCache_;
};

#endif // LOADING_DIALOG_H
