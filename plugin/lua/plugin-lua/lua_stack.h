#ifndef LUA_STACK_H
#define LUA_STACK_H

extern "C" {
#include <lua.h>
#include <tolua++.h>
}

#include <memory>
#include <string>

template <class T>
void object_to_luaval(lua_State* L, const char* type, T* ret);

namespace pip
{
    /** LuaStack
     *  LuaStack在构造的时候会记录下当前Lua栈顶的位置，析构的时候会恢复栈顶位置。
     *  注意：在LuaStack作用域内的所有Lua元素，都会被自动回收。
     */
    class LuaStack
    {
    public:
        // use LuaEngine.
        LuaStack();

        LuaStack(lua_State *L)
            : state_(L)
        {
            top_ = lua_gettop(state_);
        }

        ~LuaStack()
        {
            lua_settop(state_, top_);
        }

        lua_State* getLuaState() { return state_; }

        // suport syntax: a.b.test
        bool pushFunction(const char * name);

        template<typename T>
        void pushObject(T *p, const char * luaType = nullptr)
        {
            object_to_luaval<T>(state_, luaType, p);
        }

        template<typename T>
        bool pushMethod(T * p, const char * method, const char * luaType = nullptr)
        {
            object_to_luaval<T>(state_, luaType, p);
            lua_getfield(state_, -1, method);

            if(lua_isfunction(state_, -1))
            {
                lua_remove(state_, -2); // remove self.
                return true;
            }
            else
            {
                lua_pop(state_, 2);
                return false;
            }
        }

        template<typename T>
        bool pushMethodForCall(T * p, const char * method, const char * luaType = nullptr)
        {
            object_to_luaval<T>(state_, luaType, p);
            lua_getfield(state_, -1, method);

            if(lua_isfunction(state_, -1))
            {
                // move this method before self,
                // so self is first argument for the method.
                lua_insert(state_, -2);
                return true;
            }
            else
            {
                lua_pop(state_, 2);
                return false;
            }
        }

        inline void newTable(){ lua_newtable(state_); }
        
        inline void setTable(int index){ lua_settable(state_, index); }
        inline void rawSet(int index){ lua_rawset(state_, index); }
        inline void rawSetI(int index, int n) { lua_rawseti(state_, index, n); }
        inline void setField(int index, const char *key){ lua_setfield(state_, index, key); }
        
        inline void pushBool(bool b){ lua_pushboolean(state_, b ? 1 : 0); }
        inline void pushInt(lua_Integer v){ lua_pushinteger(state_, v);}
        inline void pushNumber(lua_Number v){ lua_pushnumber(state_, v); }
        inline void pushString(const char *s){ lua_pushstring(state_, s); }
        inline void pushString(const char *s, size_t l){ lua_pushlstring(state_, s, l); }
        inline void pushString(const std::string & s){ lua_pushlstring(state_, s.c_str(), s.size());}

        bool doCall(int numArgs, int numReturns = 0);

        /** 执行lua函数
         *  如果执行成功，返回true，并且堆栈上留下numReturns个返回值。
         *  如果执行失败，返回false，堆栈为空。
         *  如果，位置-(numArgs + 1)处的元素不是函数，程序返回false，
         *      并且清除掉numArgs + 1个栈顶元素。
         */
        static bool callFunction(lua_State *L, int numArgs, int numReturns = 0);

    private:
        lua_State*  state_;
        int         top_;
    };
    
    
    class FunctionHolder
    {
    public:
        FunctionHolder(lua_State *L, int function);
        ~FunctionHolder();
        
        bool push();
        
    protected:
        std::shared_ptr<int>    handle_;
        lua_State*              state_;
    };

} // end namespace pip

#endif // LUA_STACK_H
