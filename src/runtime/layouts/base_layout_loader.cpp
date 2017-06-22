#include "base_layout_loader.h"
#include "../object_factory.h"
#include "../json_helper.h"

BaseLayoutLoader::BaseLayoutLoader()
{

}

BaseLayoutLoader::~BaseLayoutLoader()
{

}

bool BaseLayoutLoader::init()
{
    return true;
}

void BaseLayoutLoader::applyProperties(ObjectLoader *loader, const JsonHandle &config, ObjectType *p)
{
    mjson::Dict *dict = config->asDict();
    for(const auto &pair : *dict)
    {
        std::string name = pair.key.asStdString();
        if(!loader->setProperty(p, name, pair.value, config))
        {
            CCLOG("Loader '%s': ignored property '%s'.", loader->getLoaderName(), name.c_str());
        }
    }
}

ObjectLoader* BaseLayoutLoader::choiceLoader(const JsonHandle &config)
{
    std::string type = json2string(config["type"]);

    ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
    if(nullptr == loader)
    {
        CCLOGERROR("Failed to find loader for type '%s'", type.c_str());
    }
    return loader;
}
