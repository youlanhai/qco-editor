#include "logout.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tools/log_event_dispatcher.h"
#include "tools/log_tool.h"
#include "functional"

namespace Editor
{

    static const QColor LogPriorityColor[] = {
        Qt::gray,
        Qt::green,
        Qt::yellow,
        Qt::yellow,
        Qt::red,
        Qt::red,
    };

    const QColor& getLogPriorityColor(int priority)
    {
        return LogPriorityColor[std::min(priority, LOG_PRIORITY_FATAL)];
    }


    LogOut::LogOut(QObject *parent) : QObject(parent)
    {
        LogEventDispatcher::instance()->subscribe("logView",std::bind(&LogOut::showViewLog,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    }

    LogOut::~LogOut()
    {
        LogEventDispatcher::instance()->unsubscribe("logView");
    }

    void LogOut::showViewLog(int priority, const char * tag, const char * log)
    {
        const char * prefix = getLogPriorityString(priority);
        QString logStr = QString("[%1][%2]: %3").arg(
                    QString::fromLatin1(prefix),
                    QString::fromLatin1(tag),
                    QString::fromUtf8(log));

        QListWidget* listWidget = MainWindow::instance()->ui->logListWidget;
        QListWidgetItem *item = new QListWidgetItem(logStr, listWidget);
        listWidget->addItem(item);
        listWidget->scrollToBottom();
        item->setTextColor(getLogPriorityColor(priority));
    }
}
