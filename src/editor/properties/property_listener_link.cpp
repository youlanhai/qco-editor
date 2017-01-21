#include "property_listener_link.h"
#include "../framework.h"
#include "../tools/json_tools.h"
#include "property_group.h"
#include "runtime/object_factory.h"
#include "runtime/object_loader.h"
#include "property_param.h"

#include <2d/CCNode.h>

namespace Editor
{
    PropertyListenerLink::PropertyListenerLink(const std::string &propertyName)
    : propertyName_(propertyName)
    {
        
    }
        
    void PropertyListenerLink::onPropertyChange(PropertyParam &param)
    {
        std::string type;
        param.properties["type"] >> type;

        ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
        if(loader == nullptr)
        {
            return;
        }

        JsonHandle value = loader->getProperty(param.object, propertyName_);
        if(value.isNull())
        {
            return;
        }
        Framework::instance()->emitObjectPropertyChange(param.object, propertyName_, *value);
    }
   
}
