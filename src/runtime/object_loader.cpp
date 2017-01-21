#include "object_loader.h"
#include "json_helper.h"

#include <base/CCRef.h>

ObjectLoader::ObjectLoader()
{

}

ObjectLoader::~ObjectLoader()
{

}

bool ObjectLoader::setProperty(ObjectType *node, const std::string &name, const JsonHandle &value, const JsonHandle &config)
{
    PropertySetterMap::iterator it = setters_.find(name);
    if(it != setters_.end())
    {
        (it->second)(node, value, config);
        return true;
    }
    return false;
}

JsonHandle ObjectLoader::getProperty(ObjectType *node, const std::string &name)
{
    PropertyGetterMap::iterator it = getters_.find(name);
    if(it != getters_.end())
    {
        return (it->second)(node);
    }
    return JsonHandle();
}

void ObjectLoader::registerPropertySetter(const std::string &propertyName, PropertySetter method)
{
    setters_[propertyName] = method;
}


void ObjectLoader::registerPropertyGetter(const std::string &propertyName, PropertyGetter method)
{
    getters_[propertyName] = method;
}

void ObjectLoader::postLoaded(ObjectType* /*node*/, const JsonHandle& /*config*/)
{

}

bool ObjectLoader::upgradeProperty(JsonHandle& /*property*/)
{
    return true;
}
