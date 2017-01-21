#include "lua_component.h"
#include <CCLuaEngine.h>
#include <LuaBasicConversions.h>
#include <platform/CCFileUtils.h>

#include "lua_helper.h"
#include "lua_stack.h"

#include "runtime/string_tool.h"

#ifdef BUILD_EDITOR
#include "editor/document.h"
#include "editor/framework.h"
#include "editor/resource_mgr.h"
#include "editor/tools/log_tool.h"
#include "editor/tools/qstring_tool.h"
#else
#define LOG_ERROR CCLOGERROR
#endif

static const char * CLASS_NAME = "cc.Component";
static const char * KEY_SCRIPT = "script";


LuaComponent::LuaComponent()
    : luaState_(nullptr)
{
}

LuaComponent::~LuaComponent()
{
    cleanupScriptFile();
}

bool LuaComponent::init()
{
    if(!cocos2d::Component::init())
    {
        return false;
    }

    luaState_ = cocos2d::LuaEngine::getInstance()->getLuaStack()->getLuaState();
    return luaState_ != nullptr;
}

void LuaComponent::cleanupScriptFile()
{
    if(pushMethod("onUnbind"))
    {
        pip::LuaStack::callFunction(luaState_, 1);
    }

    pushSelf();
    lua_pushnil(luaState_);
    lua_setfield(luaState_, -2, KEY_SCRIPT);
}

bool LuaComponent::loadScriptFile(const std::string &scriptFile, lua_State *L, bool reload)
{
    auto stack = LuaEngine::getInstance()->getLuaStack();
    if(nullptr == L)
    {
        L = stack->getLuaState();
    }

    std::string moduleName = scriptFile;
#ifdef BUILD_EDITOR
    QString relativePath = Editor::Framework::instance()->resource_->toResourcePath(QString::fromStdString(moduleName));
    if(!relativePath.isEmpty())
    {
        moduleName = relativePath.toStdString();
    }
#endif
    pip::removeFileExt(moduleName);
    std::replace(moduleName.begin(), moduleName.end(), '/', '.');
    std::replace(moduleName.begin(), moduleName.end(), '\\', '.');

    if(reload)
    {
        cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(scriptFile);
        if(data.isNull())
        {
            LOG_ERROR("Failed to read script file %s", scriptFile.c_str());
            return false;
        }

        // parse script
        if(0 != stack->luaLoadBuffer(L, (const char*)data.getBytes(), (int)data.getSize(), scriptFile.c_str()))
        {
            LOG_ERROR("Failed to parse script file %s: %s", scriptFile.c_str(), lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }

        LOG_DEBUG("Reload lua module '%s'", moduleName.c_str());
    }
    else
    {
        lua_getglobal(L, "require");
    }

    // execute script
    lua_pushcppstring(L, moduleName);
    if(!pip::LuaStack::callFunction(L, 1, 1))
    {
        LOG_ERROR("Failed to execute script file %s", scriptFile.c_str());
        return false;
    }

    // 脚本返回值必须是一个类
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("LuaComponent::loadScriptFile: script file '%s'' must return a table", scriptFile.c_str());
        lua_pop(L, 1);// pop the result.
        return false;
    }

    if(reload)
    {
        // 重写已加载的类
        int top = lua_gettop(L);

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "loaded");
        lua_pushstring(L, moduleName.c_str());
        lua_pushvalue(L, top);
        lua_settable(L, -3);

        lua_settop(L, top);
    }

    return true;
}

void LuaComponent::setScriptFile(const std::string &scriptFile)
{
    if(scriptFile_ == scriptFile)
    {
        return;
    }

    cleanupScriptFile();
    scriptFile_ = scriptFile;

    if(scriptFile.empty())
    {
        return;
    }

#ifdef BUILD_EDITOR
    // rename
    std::string newName = pip::getFileName(scriptFile);
    pip::removeFileExt(newName);
    Editor::Framework::instance()->document_->renameComponent(this, newName);
#endif

    LUA_STACK_CHECKER(luaState_);
    loadScript(scriptFile_);
}

bool LuaComponent::loadScript(const std::string &scriptFile)
{
    lua_State *L = luaState_;
    // lstack 自动管理堆栈
    pip::LuaStack lstack(L);

    if(!loadScriptFile(scriptFile))
    {
        return false;
    }
    // now, the class table is on top

    // find the method 'new', to create a new instance.
    // class.new()
    lua_getfield(L, -1, "new");
    if(lua_isfunction(L, -1))
    {
        if(!lstack.doCall(0, 1))
        {
            return false;
        }
        if(!lua_istable(L, -1) && !lua_isuserdata(L, -1))
        {
            LOG_ERROR("Invalid 'new'' method, an instance should be returned.");
            return false;
        }
    }
    else if(lua_isnil(L, -1))
    {
        lua_pop(L, 1); // pop the nil value

        // create a new instance.
        lua_newtable(L);

        // metatable = {__index = class }
        lua_newtable(L); // metatable
        lua_pushvalue(L, -3); // duplicate the module table.
        lua_setfield(L, -2, "__index");

        lua_setmetatable(L, -2);
    }
    else
    {
        LOG_ERROR("LuaComponent::loadScript: invalid property 'new', function type expected.");
        return false;
    }

    // script.component = self
    pushSelf();
    lua_setfield(L, -2, "component");

    // self.script = script
    pushSelf();
    lua_pushvalue(L, -2); //duplicate the new instance
    lua_setfield(L, -2, KEY_SCRIPT);
    lua_pop(L, 1); // pop self

    // script:onBind()
    lua_getfield(L, -1, "onBind");
    if(lua_isfunction(L, -1))
    {
        // move the function before instance.
        lua_insert(L, -2);
        lstack.callFunction(L, 1);
    }

    return true;
}

void LuaComponent::onEnter()
{
    if(pushMethod("onEnter"))
    {
        pip::LuaStack::callFunction(luaState_, 1);
    }
}

void LuaComponent::onExit()
{
    if(pushMethod("onExit"))
    {
        pip::LuaStack::callFunction(luaState_, 1);
    }
}

void LuaComponent::onAdd()
{
    if(pushScript())
    {
        object_to_luaval(luaState_, "cc.Node", getOwner());
        lua_setfield(luaState_, -2, "node");
        lua_pop(luaState_, 1);
    }

    if(pushMethod("onAdd"))
    {
        pip::LuaStack::callFunction(luaState_, 1);
    }
}

void LuaComponent::onRemove()
{
    if(pushScript())
    {
        lua_pushnil(luaState_);
        lua_setfield(luaState_, -2, "node");
        lua_pop(luaState_, 1);
    }

    if(pushMethod("onRemove"))
    {
        pip::LuaStack::callFunction(luaState_, 1);
    }
}

bool LuaComponent::pushMethod(const char *function)
{
    if(!pushScript())
    {
        return false;
    }

    lua_getfield(luaState_, -1, function);
    if(!lua_isfunction(luaState_, -1))
    {
        lua_pop(luaState_, 2);
        return false;
    }

    // move function before instance
    lua_insert(luaState_, -2);
    return true;
}

void LuaComponent::pushSelf()
{
    object_to_luaval(luaState_, CLASS_NAME, this);
}

bool LuaComponent::pushScript()
{
    pushSelf();
    lua_getfield(luaState_, -1, KEY_SCRIPT);
    if(!lua_istable(luaState_, -1))
    {
        lua_pop(luaState_, 2);
        return false;
    }
    else
    {
        lua_remove(luaState_, -2);
        return true;
    }
}

void LuaComponent::setProperties(const JsonHandle &config)
{
    if(!pushScript())
    {
        return;
    }

    mjson::Dict *dict = config->asDict();
    for(const auto &pair : *dict)
    {
        lua_pushlstring(luaState_, pair.key.asCString(), pair.key.size());
        lua_pushjson(luaState_, pair.value);
        lua_settable(luaState_, -3);
    }
    lua_pop(luaState_, 1); // pop script
}

void LuaComponent::setProperty(const std::string &key, const JsonHandle &value)
{
    if(!pushScript())
    {
        return;
    }

    lua_pushlstring(luaState_, key.c_str(), key.size());
    lua_pushjson(luaState_, *value);
    lua_settable(luaState_, -3);
    lua_pop(luaState_, 1); // pop script
}
