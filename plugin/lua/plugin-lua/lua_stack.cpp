#include "lua_stack.h"
#include "LuaBasicConversions.h"
#include "CCLuaEngine.h"

namespace pip
{
    /*static*/ int defaultErrorFun(lua_State *L)
    {
        CCLOGERROR("Script Error: %s", lua_tostring(L, -1));
        return 0;
    }

    void pushErrorHandleFunction(lua_State *L)
    {
        lua_getglobal(L, "__G__TRACKBACK__");
        if (!lua_isfunction(L, -1))
        {
            lua_pop(L, 1);
            // use default error function.
            lua_pushcfunction(L, defaultErrorFun);
        }
    }

    LuaStack::LuaStack()
    {
        cocos2d::LuaStack * stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
        state_ = stack->getLuaState();
        top_ = lua_gettop(state_);
    }

    bool LuaStack::pushFunction(const char * name)
    {
        lua_pushvalue(state_, LUA_GLOBALSINDEX);
        while(*name)
        {
            const char *p = strchr(name, '.');
            if(p == nullptr)
            {
                p = name + strlen(name);
            }

            lua_pushlstring(state_, name, p - name);
            lua_gettable(state_, -2);
            name = p + 1;
        }

        if(lua_isfunction(state_, -1))
        {
            lua_remove(state_, -2);
            return true;
        }
        else
        {
            lua_pop(state_, 2);
            return false;
        }
    }

    bool LuaStack::doCall(int numArgs, int numReturns)
    {
        return callFunction(state_, numArgs, numReturns);
    }

    /*static*/ bool LuaStack::callFunction(lua_State *L, int numArgs, int numReturns)
    {
        int functionIndex = -(numArgs + 1);
        if (!lua_isfunction(L, functionIndex))
        {
            CCLOGERROR("Lua value at stack [%d] is not a function", functionIndex);
            lua_pop(L, numArgs + 1);
            return false;
        }

        // push the error handler, and move it before the function.
        pushErrorHandleFunction(L);
        lua_insert(L, functionIndex - 1);

        int ret = lua_pcall(L, numArgs, numReturns, functionIndex - 1);
        if(ret != 0)
        {
            CCLOGERROR("Failed to call lua function.");
            // remove error message and error function
            lua_pop(L, 2);
        }
        else
        {
            // remove error function
            lua_remove(L, -(numReturns + 1));
        }

        return ret == 0;
    }
    
    
    FunctionHolder::FunctionHolder(lua_State *L, int function)
    : state_(L)
    {
        int ret = toluafix_ref_function(L, function, 0);
        if( ret != 0)
        {
            handle_ = std::shared_ptr<int>(new int(ret));
        }
    }
    
    FunctionHolder::~FunctionHolder()
    {
        if(handle_ && handle_.unique())
        {
            toluafix_remove_function_by_refid(state_, *handle_);
        }
    }
    
    bool FunctionHolder::push()
    {
        if(handle_)
        {
            toluafix_get_function_by_refid(state_, *handle_);
            return true;
        }
        return false;
    }

} // end namespace pip
