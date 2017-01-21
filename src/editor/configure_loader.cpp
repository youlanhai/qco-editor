#include "configure_loader.h"
#include "tools/log_tool.h"
#include "tools/file_tools.h"
#include <runtime/json_helper.h>

#include <QFile>

IMPLEMENT_SINGLETON(Editor::ConfigureLoader);


namespace Editor
{
    extern void registerDefaultConfigureLoaders(ConfigureLoader *);

    ConfigureLoader::ConfigureLoader()
    {
        registerDefaultConfigureLoaders(this);
    }

    bool ConfigureLoader::tryAddConfigure(const QString &path)
    {
        if(QFile::exists(path))
        {
            return addConfigure(path);
        }
        return true;
    }

    bool ConfigureLoader::addConfigure(const QString &path)
    {
        std::string utf8Path = path.toStdString();
        LOG_INFO("Add Configure file %s", utf8Path.c_str());

        JsonValue doc;
        if(!openJsonFile(utf8Path, doc) || !doc.isArray())
        {
            return false;
        }
        std::string rootPath = getFilePath(utf8Path);

        for(unsigned i = 0; i < doc.size(); ++i)
        {
            JsonHandle handle = doc[i];
            if(!handle.isArray() || handle->size() != 2)
            {
                LOG_ERROR("Inviliad configure value at index %u, string array(2) was expected.", i);
                return false;
            }

            std::string type, fileName;
            handle[0u] >> type;
            handle[1u] >> fileName;

            fileName = joinPath(rootPath, fileName);
            LOG_DEBUG("Load configure '%s': '%s'", type.c_str(), fileName.c_str());

            ConfigureLoaderMethod method = loaders_.value(QString::fromStdString(type));
            if(method == nullptr)
            {
                LOG_ERROR("Configure type '%s' was not registered.", type.c_str());
                return false;
            }

            if(!method(fileName))
            {
                LOG_ERROR("Failed to load configure file '%s'", fileName.c_str());
                return false;
            }
        }

        return true;
    }

    void ConfigureLoader::registerLoader(const QString &type, ConfigureLoaderMethod method)
    {
        loaders_[type] = method;
    }
}
