#ifndef PLUGIN_LUA_H
#define PLUGIN_LUA_H

#include "editor/plugin_interface.h"

namespace Editor
{
    class PluginLua : public PluginInterface
    {
        Q_OBJECT
    public:
        PluginLua();
        ~PluginLua();

        virtual bool init() override;
    };
}

#endif //PLUGIN_LUA_H
