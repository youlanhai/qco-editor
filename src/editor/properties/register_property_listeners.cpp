#include "register_property_listeners.h"
#include "editor/properties/property_group.h"
#include "property_listener_widget.h"
#include "property_listener_tile.h"

namespace Editor
{

    void registerPropertyListeners()
    {
        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup("Widget");
        if(group != nullptr)
        {
            group->addListener("position", new PropertyListenerWidgetPosition());
            group->addListener("size", new PropertyListenerWidgetSize());
            group->addListener("layoutComponentEnable", new PropertyListenerWidgetLayout());
        }

        group = PropertyGroupMgr::instance()->findGroup("TileCollisionNode");
        if(group != nullptr)
        {
            group->addListener("images", new PropertyListenerTile());
        }
    }

}
