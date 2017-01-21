#ifndef PLUGIN_MGR_H
#define PLUGIN_MGR_H

#include "config.h"
#include <QObject>
#include <QHash>

class QDir;

namespace Editor
{
    class PluginInterface;

    class EDITOR_DLL PluginMgr : public QObject
    {
        Q_OBJECT
    public:
        typedef QHash<QString, PluginInterface*> Plugins;

        PluginMgr(QObject *parent=0);
        ~PluginMgr();

        bool loadPlugins();

        void addPlugin(const QString &name, PluginInterface *plugin);
        void removePlugin(const QString &name);
        void removePlugin(PluginInterface *plugin);

        Plugins getPlugins(){ return plugins_; }

    private:
        void initPlugin(const QString &name);
        void loadPlugins(QDir &dir);
        bool loadPlugin(const QString &name, const QString &libraryPath);

        QHash<QString, PluginInterface*>   plugins_;
    };

}

#endif // PLUGIN_MGR_H
