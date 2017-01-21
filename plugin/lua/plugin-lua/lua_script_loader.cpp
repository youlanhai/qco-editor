#include "lua_script_loader.h"

#include "lua_component.h"

#include "runtime/json_helper.h"

#ifdef BUILD_EDITOR
DECLARE_PROPERTY_SETTER(LuaComponent, scriptFile, setScriptFile, ResourceHolder)
#endif

LuaScriptLoader::LuaScriptLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(scriptFile);
#endif
}

ObjectType *LuaScriptLoader::createObject(const JsonHandle &config)
{
    std::string path;
    config["scriptFile"] >> path;
    RESOLVE_FILE_RESOURCE(path);

    LuaComponent *p = LuaComponent::create();
    p->setScriptFile(path);
    p->setProperties(config);
    return p;
}
