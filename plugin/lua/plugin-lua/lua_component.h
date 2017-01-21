#ifndef EDITOR_LUA_COMPONENT_H
#define EDITOR_LUA_COMPONENT_H

#include <2d/CCComponent.h>
#include "runtime/json_helper.h"

typedef struct lua_State lua_State;

void lua_pushjson(lua_State *L, const JsonHandle &value);

class LuaComponent : public cocos2d::Component
{
public:
    CREATE_FUNC(LuaComponent)

    ~LuaComponent();

    void setProperties(const JsonHandle &config);
    void setProperty(const std::string &key, const JsonHandle &value);

    void setScriptFile(const std::string &scriptFile);
    const std::string& getScriptFile() const { return scriptFile_; }

    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void onAdd() override;
    virtual void onRemove() override;

    /**
     *  加载脚本文件，返回类。
     *  @param scriptFile 脚本文件的路径。如果是编辑器模式，可以接受绝对路径；否则必须是相对路径。
     *  @param reload 是否重新加载脚本。如果是false，使用require来加载脚本；否则重新加载脚本。
     *  @return 如果脚本加载成功，将加载得到的类置于栈顶，并返回true；否则，不修改栈，返回false。
     */
    static bool loadScriptFile(const std::string &scriptFile, lua_State *L = nullptr, bool reload = false);

protected:
    LuaComponent();
    bool loadScript(const std::string &scriptFile);
    void cleanupScriptFile();
    bool pushMethod(const char *function);
    void pushSelf();
    bool pushScript();

    std::string         scriptFile_;
    lua_State*          luaState_;
};

#endif //EDITOR_LUA_COMPONENT_H
