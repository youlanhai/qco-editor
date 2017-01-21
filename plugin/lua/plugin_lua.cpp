#include "plugin_lua.h"

#include "qt_script_property.h"
#include "plugin-lua/lua_script_loader.h"

#include "runtime/object_factory.h"

#include "editor/properties/property_creator.h"
#include "editor/inspector.h"
#include "editor/framework.h"
#include "editor/configure_loader.h"

#include "editor/properties/qt_comp_property.h"

#include <qtpropertyeditor.h>
#include <qtpropertyeditorfactory.h>

#include <CCLuaEngine.h>
#include <lua_module_register.h>

DEFINE_PLUGIN(lua, Editor::PluginLua)

namespace Editor
{
    static const QtPropertyType::Type TYPE_SCRIPT = "luaScript";

    PluginLua::PluginLua()
    {

    }

    PluginLua::~PluginLua()
    {

    }

    bool PluginLua::init()
    {
        PluginInterface::init();

        // register lua module
        auto engine = cocos2d::LuaEngine::getInstance();
        cocos2d::ScriptEngineManager::getInstance()->setScriptEngine(engine);
        lua_State* L = engine->getLuaStack()->getLuaState();
        lua_module_register(L);

        lua_pushboolean(L, 1);
        lua_setglobal(L, "BUILD_EDITOR");


        ObjectFactory::instance()->registerLoader("LuaScript", new LuaScriptLoader());

        QtPropertyFactory *propertyFactory = PropertyCreatorMgr::instance()->getPropertyMgr();
        propertyFactory->registerSimpleCreator<QtScriptProperty>(TYPE_SCRIPT);

        QtPropertyEditorFactory *editFactory = Framework::instance()->inspector_->getPropertyEditorFactory();
        editFactory->registerCreator<QtComponentEditor>(TYPE_SCRIPT);

        if(!ConfigureLoader::instance()->addConfigure("plugin/lua/configure.json"))
        {
            return false;
        }

        engine->executeScriptFile("plugin/lua/functions.lua");
        return true;
    }
}
