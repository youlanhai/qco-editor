#include "component_loader_mgr.h"
#include "object_loader.h"
#include "object_factory.h"
#include "json_helper.h"

#include <2d/CCNode.h>
#include <2d/CCComponent.h>
#include <base/ccMacros.h>

IMPLEMENT_SINGLETON(ComponentLoaderMgr);

ComponentLoaderMgr::ComponentLoaderMgr()
{
}

ComponentLoaderMgr::~ComponentLoaderMgr()
{
}

cocos2d::Component* ComponentLoaderMgr::create(const JsonHandle &config, std::string name)
{
    std::string type;
    config["type"] >> type;

    ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
    if(nullptr == loader)
    {
        CCLOGERROR("The component loader for type '%s' was not found.", type.c_str());
        return nullptr;
    }

    cocos2d::Component *p = dynamic_cast<cocos2d::Component*>(loader->createObject(config));
    if(nullptr == p)
    {
        CCLOGERROR("Failed to create component for type '%s'", type.c_str());
        return nullptr;
    }

    if(!name.empty())
    {
        p->setName(name);
    }

    if(p->getName().empty())
    {
        p->setName(type);
    }
    return p;
}

void ComponentLoaderMgr::setProperties(cocos2d::Component *p, const JsonHandle &config)
{
    ObjectLoader *loader = ObjectFactory::instance()->findLoader(config["type"]->asStdString());
    if(nullptr == loader)
    {
        return;
    }

    const mjson::Dict &dict = config->refDict();
    for(const auto &pair : dict)
    {
        if(!loader->setProperty(p, pair.key.asStdString(), pair.value, config))
        {
            CCLOG("WARN: component property '%s' was ignored.", pair.key.asCString());
        }
    }
}

cocos2d::Component* ComponentLoaderMgr::load(const JsonHandle &config, const std::string &name, cocos2d::Node *owner)
{
    cocos2d::Component *p = this->create(config, name);
    if(nullptr == p)
    {
        return nullptr;
    }

    if(owner->getComponent(p->getName()) != nullptr)
    {
        CCLOGERROR("Component with name '%s' has been exist.", p->getName().c_str());
        return nullptr;
    }
    owner->addComponent(p);

    setProperties(p, config);
    return p;
}
