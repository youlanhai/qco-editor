#include "configure_loader.h"
#include "framework.h"
#include "properties/property_group.h"
#include "properties/property_default.h"
#include "properties/property_creator.h"
#include "menu_loader.h"
#include "hierarchy.h"
#include "runtime/resource_mapping.h"

namespace Editor
{
    namespace
    {
        bool loadPropertyValue(const std::string &path)
        {
            return PropertyCreatorMgr::instance()->registerCreatorFile(path);
        }
        bool loadPropertyType(const std::string &path)
        {
            return PropertyGroupMgr::instance()->registerGroupFile(path);
        }
        bool loadPropertyDefault(const std::string &path)
        {
            return PropertyDefault::instance()->loadDefault(path);
        }
        bool loadObjectMenu(const std::string &path)
        {
            if(!Framework::instance()->objectMenu_->loadMenu(path))
            {
                return false;
            }
            if(!Framework::instance()->hierarchy_->loadNewMenu(path))
            {
                return false;
            }
            return true;
        }
        bool loadComponentMenu(const std::string &path)
        {
            return Framework::instance()->componentMenu_->loadMenu(path);
        }

        bool loadFontConfigure(const std::string &path)
        {
            return ResourceMapping::instance()->font.addConfigFile(path);
        }

        bool loadTextConfigure(const std::string &path)
        {
            return ResourceMapping::instance()->text.addConfigFile(path);
        }

        bool loadFileConfigure(const std::string &path)
        {
            return ResourceMapping::instance()->file.addConfigFile(path);
        }
    }

    void registerDefaultConfigureLoaders(ConfigureLoader *configLoader)
    {
        configLoader->registerLoader("property_type", loadPropertyType);
        configLoader->registerLoader("property_value", loadPropertyValue);
        configLoader->registerLoader("property_default", loadPropertyDefault);
        configLoader->registerLoader("object_menu", loadObjectMenu);
        configLoader->registerLoader("component_menu", loadComponentMenu);
        configLoader->registerLoader("font", loadFontConfigure);
        configLoader->registerLoader("text", loadTextConfigure);
        configLoader->registerLoader("file", loadFileConfigure);
    }
}
