#include "plugin_interface.h"

namespace Editor
{
    PluginInterface::PluginInterface()
        : name_("no-name")
        , initialized_(false)
    {

    }

    PluginInterface::~PluginInterface()
    {

    }

    bool PluginInterface::init()
    {
        initialized_ = true;
        return true;
    }
}
