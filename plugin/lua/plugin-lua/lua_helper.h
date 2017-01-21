#ifndef __client__LuaHelper__
#define __client__LuaHelper__

#include <base/ccMacros.h>
#include <cstdarg>
#include <unordered_map>
#include <smartjson/src/json.hpp>

extern "C" {
#include <lua.h>
#include <tolua++.h>
}

extern std::unordered_map<std::string, std::string>  g_luaType;
extern std::unordered_map<std::string, std::string>  g_typeCast;

#define LUA_FUN_HEADER(NAME) static int NAME(lua_State *L)

/** parse arguments according to the format.
 @code
 int age;
 const char * name;
 if(!lua_parse_args(L, "is", &age, &name)) return false;
 @endcode
 *
 *  @param format: is a string, contains the following characters.
 *  b: boolean
 *  c: char
 *  C: unsingend char
 *  h: short
 *  H: unsigned short
 *  i: int
 *  I: unsigned int
 *  l: long
 *  L: unsigned long
 *  v: long long
 *  V: unsigned long long
 *  f: float
 *  d: double
 *  s: char*, c string
 *  S: std::string, stl string
 *  F: function
 *  t: table
 *  T: usertype
 *  T(TypeName): usertype with typename
 *  u: userdata
 *  U(TypeName): usertable with typename
 *  *: anything the lua supported
 *  .: placeholder
 *  |: means the following arguments are default
 *  ?: ignore the remain arguments
 *
 *  @return return true if parse succed, else false.
 */
bool lua_parse_args(lua_State *L, const char * format, ...);

#define LUA_PARSE_METHOD(L, format, ...) \
if(!lua_parse_args(L, format, __VA_ARGS__, nullptr) || !self){\
    CCLOGERROR("Failed to parse method %s", __FUNCTION__ );\
    return 0;\
}

#define LUA_PARSE_FUN(L, format, ...)\
if(!lua_parse_args(L, format, __VA_ARGS__, nullptr)){\
    CCLOGERROR("Failed to parse function %s", __FUNCTION__ );\
    return 0;\
}

void lua_pushjson(lua_State *L, const mjson::Node &value);
void lua_pushcppstring(lua_State *L, const std::string &value);

template<typename T>
void registerType(const char * cls)
{
    std::string typeName = typeid(T).name();
    auto pair = std::pair<std::string, std::string>(typeName, cls);
    auto ret = g_luaType.insert(pair);

    if(!ret.second)
    {
        CCLOGERROR("The lua class '%s' has been exist, c++ class is '%s'.", cls, typeName.c_str());
    }
}

template<typename T>
void lua_setfield_number(lua_State *L, const char *name, T value)
{
    lua_pushnumber(L, (lua_Number)value);
    lua_setfield(L, -2, name);
}
void lua_setfield_bool(lua_State *L, const char *name, bool value);
void lua_setfield_string(lua_State *L, const char *name, const char *value);
void lua_setfield_string(lua_State *L, const char *name, const std::string &value);



class LuaStackChecker
{
    lua_State*  state_;
    int         top_;
    const char* file_;
    const char* func_;
    int         line_;
public:
    LuaStackChecker(lua_State *L, const char *file, const char *func, int line);
    ~LuaStackChecker();
};

#if COCOS2D_DEBUG > 0
#define LUA_STACK_CHECKER(L)    LuaStackChecker _luaStackChecker(L, __FILE__, __FUNCTION__, __LINE__)
#else
#define LUA_STACK_CHECKER(L)
#endif


#endif /* defined(__client__LuaHelper__) */
