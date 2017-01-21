#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include "config.h"
#include <QObject>

#define DEFINE_PLUGIN(NAME, CLASS) \
extern "C" Q_DECL_EXPORT void* createPlugin_##NAME() \
{ \
    return new CLASS(); \
}

namespace Editor
{
    class EDITOR_DLL PluginInterface : public QObject
    {
        Q_OBJECT
    public:
        PluginInterface();
        virtual ~PluginInterface();

        virtual bool init();

        bool isInitialized() const { return initialized_; }
        const QString getName() const { return name_; }
        const QString getPath() const { return path_; }

        /** 依赖的组件需要先初始化。格式: a;b;c */
        virtual QString getDependency() const { return QString(); }

    private:
        QString     name_;
        QString     path_;
        bool        initialized_;

        friend class PluginMgr;
    };
}

#endif // PLUGIN_INTERFACE_H
