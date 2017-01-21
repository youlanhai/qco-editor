#ifndef CONFIGURE_LOADER_H
#define CONFIGURE_LOADER_H

#include "config.h"
#include <runtime/singleton.h>
#include <QString>
#include <QHash>
#include <string>

namespace Editor
{
    typedef bool(*ConfigureLoaderMethod)(const std::string &path);

    class EDITOR_DLL ConfigureLoader : public Singleton<ConfigureLoader>
    {
    public:
        ConfigureLoader();

        bool tryAddConfigure(const QString &path);
        bool addConfigure(const QString &path);

        void registerLoader(const QString &type, ConfigureLoaderMethod method);

    private:
        typedef QHash<QString, ConfigureLoaderMethod> Loaders;
        Loaders     loaders_;
    };

}

#endif // CONFIGURE_LOADER_H
