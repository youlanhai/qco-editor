#include "layout_loader_mgr.h"

#include <2d/CCNode.h>
#include <2d/CCComponent.h>
#include <platform/CCFileUtils.h>

#include "object_factory.h"
#include "json_helper.h"
#include "object_loader.h"
#include "component_loader_mgr.h"

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(LayoutLoaderMgr);

LayoutLoaderMgr::LayoutLoaderMgr()
    : version_(1)
{
}

LayoutLoaderMgr::~LayoutLoaderMgr()
{
}

ObjectType* LayoutLoaderMgr::loadFromFile(const std::string &filename, ObjectType *p)
{
    JsonDocument document;
    if(!openJsonFile(filename, document) || !document.isDict())
    {
        return NULL;
    }
    
    if(!upgradeConfig(document))
    {
        CCLOGERROR("Failed to upgrade layout file '%s'", filename.c_str());
        return NULL;
    }
    
    return loadFromConfig(document, p);
}

ObjectType* LayoutLoaderMgr::loadFromConfig(const JsonHandle &config, ObjectType *p)
{
    ObjectLoader *loader = choiceLoader(config);
    if(nullptr == loader)
    {
        return nullptr;
    }
    
    if(nullptr == p)
    {
        p = loader->createObject(config);
        if(nullptr == p)
        {
            CCLOGERROR("Failed to create object. loader is '%s'", loader->getLoaderName());
            return nullptr;
        }
    }

    loadProperty(loader, config, p);

    cocos2d::Node *pNode = dynamic_cast<cocos2d::Node*>(p);
    if(pNode != nullptr)
    {
        loadChildren(loader, config, pNode);
        loadComponents(loader, config, pNode);
    }

    loader->postLoaded(p, config);
    return p;
}

ObjectLoader* LayoutLoaderMgr::choiceLoader(const JsonHandle &config)
{
    std::string type = json2string(config["type"]);

    ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
    if(nullptr == loader)
    {
        CCLOGERROR("Failed to find loader for type '%s'", type.c_str());
    }
    return loader;
}

void LayoutLoaderMgr::loadProperty(ObjectLoader *loader, const JsonHandle &config, ObjectType *p)
{
    mjson::Dict *dict = config->asDict();
    for(const auto &pair : *dict)
    {
        std::string name = pair.key.asStdString();
        if (name == "children" ||
            name == "type" ||
            name == "version" ||
            name == "sceneType" ||
            name == "components")
        {
            continue;
        }

        if(!loader->setProperty(p, name, pair.value, config))
        {
            CCLOG("Loader '%s': ignored property '%s'.", loader->getLoaderName(), name.c_str());
        }
    }
}

void LayoutLoaderMgr::loadChildren(ObjectLoader * /*loader*/, const JsonHandle & config, cocos2d::Node *p)
{
    JsonHandle children = config["children"];
    if(children.isArray())
    {
        mjson::Array *array = children->asArray();
        for(const mjson::Node &node : *array)
        {
            cocos2d::Node *child = dynamic_cast<cocos2d::Node*>(loadFromConfig(node));
            if(child != nullptr)
            {
                p->addChild(child);
            }
        }
    }
}

void LayoutLoaderMgr::loadComponents(ObjectLoader* /*loader*/, const JsonHandle &config, cocos2d::Node *owner)
{
    JsonHandle component = config["components"];
    if(component.isObject())
    {
        auto componentMgr = ComponentLoaderMgr::instance();

        const mjson::Dict &dict = component->refDict();
        for(const auto &pair : dict)
        {
            componentMgr->load(pair.value, pair.key.asStdString(), owner);
        }
    }
}

bool LayoutLoaderMgr::upgradeLayoutFile(JsonHandle document)
{
    int version = json2int(document["version"]);
    if(version < version_)
    {
        if(!upgradeConfig(document))
        {
           return false;
        }

        document.setMember("version", JsonValue(version_));
    }
    return true;
}

bool LayoutLoaderMgr::upgradeConfig(JsonHandle config)
{
    ObjectLoader *loader = choiceLoader(config);
    if(nullptr == loader || !loader->upgradeProperty(config))
    {
        return false;
    }
    
    JsonHandle jchildren = config["children"];
    if(jchildren.isArray())
    {
        mjson::Array *array = jchildren->asArray();
        for(mjson::Node &node : *array)
        {
            if(!upgradeConfig(node))
            {
                return false;
            }
        }
    }
    
    return true;
}

#ifdef BUILD_EDITOR
bool LayoutLoaderMgr::saveToFile(const std::string & filename, JsonHandle config)
{
    trimConfigInternal(config);

    config.setMember("version", JsonHandle::value_type(version_));
    if(!saveJsonFile(filename, *config))
    {
        CCLOGERROR("Failed to save layout to file '%s'", filename.c_str());
        return false;
    }
    return true;
}

bool LayoutLoaderMgr::trimConfigInternal(JsonHandle config)
{
    ObjectLoader *loader = choiceLoader(config);
    if(NULL == loader)
    {
        return false;
    }
    
    mjson::Dict *dict = config->asDict();
    if(dict == nullptr)
    {
        return false;
    }

    mjson::Dict::iterator it = dict->find("children");
    if(it != dict->end())
    {
        if(it->value.size() == 0)
        {
            dict->erase(it);
        }
        else
        {
            mjson::Array &array = it->value.refArray();
            for(JsonValue &child : array)
            {
                if(!trimConfigInternal(child))
                {
                    return false;
                }
            }
        }
    }
    
    return true;
}
#endif
