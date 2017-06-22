#include "layout_loader_v1.h"


#include <2d/CCNode.h>
#include <2d/CCComponent.h>
#include <platform/CCFileUtils.h>

#include "../object_factory.h"
#include "../json_helper.h"
#include "../object_loader.h"
#include "../component_loader_mgr.h"

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

LayoutLoaderV1::LayoutLoaderV1()
{

}

ObjectType* LayoutLoaderV1::loadFromConfig(const JsonHandle &config, ObjectType *p)
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

    applyProperties(loader, config, p);

    cocos2d::Node *pNode = dynamic_cast<cocos2d::Node*>(p);
    if(pNode != nullptr)
    {
        loadChildren(loader, config, pNode);
        loadComponents(loader, config, pNode);
    }

    loader->postLoaded(p, config);
    return p;
}

void LayoutLoaderV1::applyProperties(ObjectLoader *loader, const JsonHandle &config, ObjectType *p)
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

void LayoutLoaderV1::loadChildren(ObjectLoader * /*loader*/, const JsonHandle & config, cocos2d::Node *p)
{
    JsonHandle children = config["children"];
    if(children.isArray())
    {
        mjson::Array *array = children->asArray();
        for(const mjson::Node &node : *array)
        {
            cocos2d::Node *child = dynamic_cast<cocos2d::Node*>(loadFromConfig(node, nullptr));
            if(child != nullptr)
            {
                p->addChild(child);
            }
        }
    }
}

void LayoutLoaderV1::loadComponents(ObjectLoader* /*loader*/, const JsonHandle &config, cocos2d::Node *owner)
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

bool LayoutLoaderV1::upgradeLayoutFile(JsonHandle document)
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

bool LayoutLoaderV1::upgradeConfig(JsonHandle config)
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
bool LayoutLoaderV1::trimConfigInternal(JsonHandle config)
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
