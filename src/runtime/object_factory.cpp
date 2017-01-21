#include "object_factory.h"
#include "object_loader.h"

#include <ccMacros.h>

IMPLEMENT_SINGLETON(ObjectFactory);

ObjectFactory::ObjectFactory()
{
}

ObjectFactory::~ObjectFactory()
{
    for(Loaders::iterator it = loaders_.begin(); it != loaders_.end(); ++it)
    {
        delete it->second;
    }
}

void ObjectFactory::registerLoader(const std::string &type, ObjectLoader *loader)
{
    CCLOG("register loader: %s", type.c_str());

    Loaders::iterator it = loaders_.find(type);
    if(it != loaders_.end())
    {
        CCLOG("The loader '%s' was covered.", type.c_str());

        delete it->second;
        it->second = loader;
    }
    else
    {
        loaders_[type] = loader;
    }
}

ObjectLoader * ObjectFactory::findLoader(const std::string &type)
{
    Loaders::iterator it = loaders_.find(type);
    if(it != loaders_.end())
    {
        return it->second;
    }

    return NULL;
}
