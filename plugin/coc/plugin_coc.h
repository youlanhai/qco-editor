#ifndef PLUGIN_COC_H
#define PLUGIN_COC_H

#include "editor/plugin_interface.h"

namespace Editor
{
    class PluginCoc : public PluginInterface
    {
        Q_OBJECT
    public:
        PluginCoc();
        ~PluginCoc();

        virtual bool init() override;
    };
}

#endif // PLUGIN_COC_H
