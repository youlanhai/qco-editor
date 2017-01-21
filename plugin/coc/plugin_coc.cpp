#include "plugin_coc.h"

#include "editor/properties/property_group.h"
#include "editor/configure_loader.h"

#include "runtime/object_factory.h"

#include "coc_animation/coc_property.h"
#include "coc_animation/coc_animation_player.h"
#include "coc_animation/coc_animation_mgr.h"
#include "coc_animation_property_listener.h"

DEFINE_PLUGIN(coc, Editor::PluginCoc)

namespace Editor
{
    static const char* CocAnimationPlayer = "CocAnimationPlayer";

    PluginCoc::PluginCoc()
    {
        coc::AnimationMgr::initInstance();
    }

    PluginCoc::~PluginCoc()
    {
        coc::AnimationMgr::finiInstance();
    }

    bool PluginCoc::init()
    {
        PluginInterface::init();

        PropertyGroupMgr::instance()->registerUIName(typeid(coc::AnimationPlayer).name(), CocAnimationPlayer);
        coc::registerCocProperty(ObjectFactory::instance());

        if(!ConfigureLoader::instance()->addConfigure("plugin/coc/configure.json"))
        {
            return false;
        }

        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(CocAnimationPlayer);
        if(group != nullptr)
        {
            group->addListener("animation", new CocAnimationPropertyListener());
        }
        return true;
    }
}
