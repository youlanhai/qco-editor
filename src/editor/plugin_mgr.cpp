#include "plugin_mgr.h"
#include "plugin_interface.h"
#include "project_mgr.h"

#include "tools/log_tool.h"
#include "tools/qstring_tool.h"

#include <QDir>
#include <QApplication>
#include <QLibrary>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "Plugin");

namespace Editor
{
    PluginMgr::PluginMgr(QObject *parent)
        : QObject(parent)
    {

    }

    PluginMgr::~PluginMgr()
    {
    	for(auto plugin : plugins_)
    	{
            delete plugin;
    	}
    }

    void PluginMgr::addPlugin(const QString &name, PluginInterface *plugin)
    {
        LOG_INFO("Register plugin: %s", name.toUtf8().data());

        PluginInterface *old = plugins_.value(name);
        if(old != nullptr)
        {
            LOG_INFO("Replace old plugin: %s", name.toUtf8().data());
            delete old;
        }

        plugin->name_ = name;
        plugins_[name] = plugin;
    }

    void PluginMgr::removePlugin(const QString &name)
    {
        auto it = plugins_.find(name);
        if(it != plugins_.end())
        {
            delete it.value();
            plugins_.erase(it);
        }
    }

    void PluginMgr::removePlugin(PluginInterface *plugin)
    {
        removePlugin(plugin->getName());
    }

    bool PluginMgr::loadPlugins()
    {
        QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(DEBUG) && defined(Q_OS_MAC)
        if(pluginsDir.dirName() == "MacOS")
        {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif
        loadPlugins(pluginsDir);

        pluginsDir = QDir(ProjectManager::instance()->getProjectPath());
        pluginsDir.cd("plugins");
        loadPlugins(pluginsDir);

        for(auto it = plugins_.begin(); it != plugins_.end(); ++it)
        {
            initPlugin(it.key());
        }
        return true;
    }

    void PluginMgr::initPlugin(const QString &name)
    {
        PluginInterface *plugin = plugins_.value(name);
        if(plugin != nullptr && !plugin->isInitialized())
        {
            // 防止循环依赖
            plugin->initialized_ = true;

            // 先初始化依赖项
            QString dependency = plugin->getDependency();
            if(!dependency.isEmpty())
            {
                QStringList depends = dependency.split(';', QString::SkipEmptyParts);
                foreach(QString name, depends)
                {
                    initPlugin(name);
                }
            }

            if(!plugin->init())
            {
                LOG_ERROR("Failed to init plugin %s", name.toUtf8().data());
            }
        }
    }

    typedef PluginInterface* (*PluginCreateFun)();

    void PluginMgr::loadPlugins(QDir &dir)
    {
        LOG_DEBUG("Plugin Path: %s", dir.path().toUtf8().data());

        if(!dir.exists())
        {
            return;
        }

        foreach(QString fileName, dir.entryList(QDir::Files))
        {
            QRegExp re(".*plugin-(\\w+).(dylib|so|dll)");
            if(re.indexIn(fileName) == -1)
            {
                continue;
            }

            QString pluginName = re.cap(1);
            QString fullPath = dir.absoluteFilePath(fileName);
            loadPlugin(pluginName, fullPath);
        }
    }

    bool PluginMgr::loadPlugin(const QString &name, const QString &libraryPath)
    {
        if(plugins_.contains(name))
        {
            return false;
        }

        std::string utf8Name = name.toStdString();

        LOG_DEBUG("Try load %s", libraryPath.toUtf8().data());
        QLibrary library(libraryPath);
        if(!library.load())
        {
            LOG_ERROR("Failed to load library '%s'", libraryPath.toUtf8().data());
            return false;
        }

        std::string funName = "createPlugin_" + utf8Name;
        PluginCreateFun fun = (PluginCreateFun)library.resolve(funName.c_str());
        if(!fun)
        {
            LOG_ERROR("Failed to find method '%s' in '%s'", funName.c_str(), libraryPath.toUtf8().data());
            return false;
        }

        PluginInterface *plugin = fun();
        if(!plugin)
        {
            LOG_WARN("Failed to create plugin %s", utf8Name.c_str());
            return false;
        }

        plugin->path_ = libraryPath;
        addPlugin(name, plugin);
        return true;
    }
}
