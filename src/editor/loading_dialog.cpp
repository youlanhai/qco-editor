#include "loading_dialog.h"
#include "ui_loadingdialog.h"

LoadingDialog::LoadingDialog(StepFunc fun, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadingDialog),
    stepFunc_(fun),
    isCanceled_(false),
    isFinished_(false)
{
    ui->setupUi(this);

    timer_.start(1);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimer()));
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}

void LoadingDialog::on_btnCancel_clicked()
{
    if(isFinished_)
    {
        accept();
    }
    else
    {
        isCanceled_ = true;
        reject();
    }
}

void LoadingDialog::onTimer()
{
    if(!isCanceled_ && !isFinished_)
    {
        if(!stepFunc_(this))
        {
            timer_.stop();
            isCanceled_ = true;
            ui->btnCancel->setText(tr("Failed"));
        }
    }

    if(!logCache_.isEmpty())
    {
        ui->labContent->setText(logCache_);
        logCache_.clear();
    }
}

void LoadingDialog::setProgress(float progress)
{
    ui->progressBar->setValue(progress * 100);
}

void LoadingDialog::showMessage(const QString &msg)
{
    logCache_ = msg;
    //ui->labContent->setText(msg);
}

void LoadingDialog::cancelLoading()
{
    isCanceled_ = true;
    ui->btnCancel->setText(tr("Canceled"));
    timer_.stop();
}

void LoadingDialog::finishLoading()
{
    isFinished_ = true;
    ui->btnCancel->setText(tr("Finished"));
    setProgress(1.0f);
    timer_.stop();
}
