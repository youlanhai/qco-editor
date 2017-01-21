#include "component_loader.h"
#include "../json_helper.h"

#include <2d/CCComponent.h>


DECLARE_PROPERTY_SETTER(cocos2d::Component, enable, setEnabled, bool)
//DECLARE_PROPERTY_SETTER(cocos2d::Component, name, setName, std::string)

ComponentLoader::ComponentLoader()
{
    REGISTER_PROPERTY_SETTER(enable);
    //REGISTER_COMPONENT_SETTER(name);
}

ObjectType* ComponentLoader::createObject(const JsonHandle& /*config*/)
{
    return cocos2d::Component::create();
}
