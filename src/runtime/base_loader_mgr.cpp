#include "base_loader_mgr.h"
#include "json_helper.h"
#include "layouts/base_layout_loader.h"


BaseLoaderMgr::BaseLoaderMgr()
{

}

BaseLoaderMgr::~BaseLoaderMgr()
{
    for(auto &pair : layoutLoaders_)
    {
        for(auto &pair2 : pair.second)
        {
            pair2.second->release();
        }
    }
}

ObjectType* BaseLoaderMgr::loadFromFile(const std::string &filename, ObjectType *p)
{
	JsonDocument document;
    if(!openJsonFile(filename, document) || !document.isDict())
    {
        return nullptr;
    }
    
    return loadFromConfig(document, p);
}

ObjectType* BaseLoaderMgr::loadFromConfig(const JsonHandle &config, ObjectType *p)
{
    std::string type = config["type"]->asStdString();
    if(type.empty())
    {
        CCLOGERROR("The file type was missing");
        return nullptr;
    }

    int version = config["version"]->asInt();
    if(0 == version)
    {
        CCLOGERROR("The file version was missing");
        return nullptr;
    }

    BaseLayoutLoader *layoutLoader = findLayoutLoader(type, version);
    if(nullptr == layoutLoader)
    {
        return nullptr;
    }

    return layoutLoader->loadFromConfig(config, p);
}

BaseLayoutLoader* BaseLoaderMgr::findLayoutLoader(const std::string &type, int version)
{
    auto it = layoutLoaders_.find(type);
    if(it != layoutLoaders_.end())
    {
        Version2Loaders &loaders = it->second;

        auto it2 = loaders.find(version);
        if(it2 != loaders.end())
        {
            return it2->second;
        }
    }

    CCLOGERROR("Failed find layout loader for type '%s', version '%d'", type.c_str(), version);
    return nullptr;
}

void BaseLoaderMgr::regiseterLayoutLoader(BaseLayoutLoader *loader)
{
    loader->retain();

    auto it = layoutLoaders_.find(loader->getType());
    if(it != layoutLoaders_.end())
    {
        Version2Loaders &loaders = it->second;
        auto it2 = loaders.find(loader->getVersion());
        if(it2 != loaders.end())
        {
            it2->second->release();
            it2->second = loader;
            return;
        }
    }

    layoutLoaders_[loader->getType()][loader->getVersion()] = loader;
}

void BaseLoaderMgr::unregisterLayoutLoader(BaseLayoutLoader *loader)
{
    auto it = layoutLoaders_.find(loader->getType());
    if(it != layoutLoaders_.end())
    {
        Version2Loaders &loaders = it->second;
        auto it2 = loaders.find(loader->getVersion());
        if(it2 != loaders.end() && loader == it2->second)
        {
            loaders.erase(it2);
            loader->release();
        }
    }
}
