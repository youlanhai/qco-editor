#ifndef LOGOUT_H
#define LOGOUT_H

#include "config.h"
#include <QObject>
#include <string>

namespace Editor
{
    class EDITOR_DLL LogOut : public QObject
    {
        Q_OBJECT
    public:
        explicit LogOut(QObject *parent = 0);
        ~LogOut();

        void showViewLog(int priority, const char * tag, const char * log);

    signals:

    public slots:

    private:
    };
}

#endif // LOGOUT_H
