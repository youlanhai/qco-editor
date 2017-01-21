#include "lua_helper.h"
#include "CCLuaEngine.h"

#include <cassert>
#include <cstring>

#define CASE_NUMBER(T, TYPE) \
case T: \
{ \
    if(!lua_isnumber(L, index))\
    {\
        luaL_error(L, "argument %d: number type needed.", index);\
        return false;\
    }\
    TYPE *p = va_arg(ap, TYPE*);\
    *p = (TYPE)lua_tonumber(L, index);\
    ++index;\
    break;\
}


extern "C" {
    extern int lua_isusertype (lua_State* L, int lo, const char* type);
}

static int lua_isusertable (lua_State* L, int lo, const char* type);

static size_t parse_type_name(char * dst, const char * src)
{
    if(*src++ != '(')
    {
        return 0;
    }
    
    // parse the type name.
    const char *pch = src;
    while(*pch != ')' && (*pch != '\0'))
    {
        ++pch;
    }
    
    if(*pch != ')')
    {
        return 0;
    }
    
    size_t length = pch - src;
    strncpy(dst, src, length);
    dst[length] = '\0';
    return length;
}

bool lua_parse_args(lua_State *L, const char * format, ...)
{
    assert(format != nullptr);
    
    const char * pch = format;
    int index = 1;
    int iTop = lua_gettop(L);
    bool isdefault = false;
    bool ignoreRemain = false;
    
    va_list ap;
    va_start(ap, format);
    
    while(*pch != '\0')
    {
        if(*pch == '?')
        {
            ignoreRemain = true;
            break;
        }
        else if(*pch == '|')
        {
            isdefault = true;
            ++pch;
            continue;
        }
        
        if(iTop < index)
        {
            if(isdefault)
            {
                break;
            }
            
            luaL_error(L, "need more than %d args, %d was given.", index, iTop);
            return false;
        }
        
        switch (*pch)
        {
            CASE_NUMBER('c', char);
            CASE_NUMBER('C', unsigned char);
            CASE_NUMBER('h', short);
            CASE_NUMBER('H', unsigned short);
            CASE_NUMBER('i', int);
            CASE_NUMBER('I', unsigned int);
            CASE_NUMBER('l', long);
            CASE_NUMBER('L', unsigned long);
            CASE_NUMBER('v', long long);
            CASE_NUMBER('V', unsigned long long);
            CASE_NUMBER('f', float);
            CASE_NUMBER('d', double);
                
            case 'b': // boolean
            {
                if(!lua_isboolean(L, index))
                {
                    luaL_error(L, "argument %d: boolean type needed.", index);
                    return false;
                }
                
                bool *p = va_arg(ap, bool*);
                *p = lua_toboolean(L, index);
                ++index;
                break;
            }
            case 's': // string
            {
                if(!lua_isstring(L, index))
                {
                    luaL_error(L, "argument %d: string type needed.", index);
                    return false;
                }
                
                const char **p = va_arg(ap, const char **);
                *p = lua_tostring(L, index);
                ++index;
                break;
            }
            case 'S': //stdstring
            {
                if(!lua_isstring(L, index))
                {
                    luaL_error(L, "argument %d: string type needed.", index);
                    return false;
                }
                std::string *p = va_arg(ap, std::string*);
                size_t length;
                const char *str = lua_tolstring(L, index, &length);
                p->assign(str, length);
                ++index;
                break;
            }
            case 't': // table
            {
                if(!lua_istable(L, index))
                {
                    luaL_error(L, "argument %d: table type needed.", index);
                    return false;
                }
                
                int *p = va_arg(ap, int*);
                *p = tolua_tovalue(L, index, 0);
                ++index;
                break;
            }
            case 'u': // user data
            {        
                const void **p = va_arg(ap, const void**);
                
                if(lua_isuserdata(L, index))
                {        
                    *p = lua_touserdata(L, index);
                }
                else if(lua_isnil(L, index))
                {
                    *p = nullptr;
                }
                else
                {
                    luaL_error(L, "argument %d: usertdata type needed.", index);
                    return false;
                }
                ++index;
                break;
            }
            case 'U': // user table
            {
                ++pch;
                char typeName[256];
                size_t length = parse_type_name(typeName, pch);
                if(length == 0)
                {
                    luaL_error(L, "argument %d: failed to parse usertable typename", index);
                    return false;
                }
                pch += length + 1; //'(' + typename
                
                if(!lua_isusertable(L, index, typeName))
                {
                    luaL_error(L, "argument %d: usertable '%s' needed.", index, typeName);
                    return false;
                }
            
                int *p = va_arg(ap, int*);
                *p = tolua_tovalue(L, index, 0);
                
                ++index;
                break;
            }
            case 'T': // user type. T or T(RealTypeName)
            {
                const void **p = va_arg(ap, const void**);
                if(*(pch + 1) == '(')
                {
                    ++pch;
                    char typeName[256];
                    size_t length = parse_type_name(typeName, pch);
                    if(length == 0)
                    {
                        luaL_error(L, "argument %d: failed to parse usertype typename", index);
                        return false;
                    }
                    pch += length + 1; //'(' + typename
                    
                    if(lua_isnil(L, index))
                    {
                        *p = nullptr;
                    }
                    else if(lua_isusertype(L, index, typeName))
                    {        
                        *p = tolua_tousertype(L, index, nullptr);
                    }
                    else
                    {
                        luaL_error(L, "argument %d: usertype type '%s' needed.", index, typeName);
                        return false;
                    }
                }
                else if(lua_isnil(L, index))
                {
                    *p = nullptr;
                }
                else if(lua_isuserdata(L, index))
                {
                    *p = tolua_tousertype(L, index, nullptr);
                }
                else
                {
                    luaL_error(L, "argument %d: usertype type needed.", index);
                    return false;
                }
                ++index;
                break;
            }
            case 'F':
            {
                if(!lua_isfunction(L, index))
                {
                    luaL_error(L, "argument %d: function type needed.", index);
                    return false;
                }
                
                int *p = va_arg(ap, int*);
                *p = tolua_tovalue(L, index, 0);
                ++index;
                break;
            }
            case '*':
            {
                int *p = va_arg(ap, int*);
                *p = tolua_tovalue(L, index, 0);
                ++index;
                break;
            }
            case '.':
            {
                ++index;
                break;
            }
            
            default:
                luaL_error(L, "argument %d: Invalid format character '%c'", index, *pch);
                return false;
        }
        
        ++pch;
    }
    
    va_end(ap);
    
    index -= 1;
    if(iTop < index || (iTop > index && !ignoreRemain))
    {
        luaL_error(L, "needed %d arguments, but %d was given.", index, iTop);
        return false;
    }
    
    return true;
}

#undef CASE_NUMBER


// copy from tolua/tolua_is.c
static int lua_isusertable (lua_State* L, int lo, const char* type)
{
    int r = 0;
    if (lo < 0) lo = lua_gettop(L)+lo+1;
    lua_pushvalue(L, lo);                   // stack: ... table
    
    // A class table must use metatable to get it's type now. 2014.6.5 by SunLightJuly
    if (lua_istable(L, -1)) {
        lua_pushliteral(L, ".isclass");
        lua_rawget(L, -2);                  // stack: ... table class_flag
        if (!lua_isnil(L, -1)) {
            lua_pop(L, 1);                  // stack: ... table
            if (lua_getmetatable(L, -1)) {  // stack: ... table mt
                lua_remove(L, -2);          // stack: ... mt
            }
        } else {
            lua_pop(L, 1);                  // stack: ... table
        }
    }
    
    lua_rawget(L,LUA_REGISTRYINDEX);  /* get registry[t] */
    if (lua_isstring(L,-1))
    {
        r = strcmp(lua_tostring(L,-1),type)==0;
        if (!r)
        {
            /* try const */
            lua_pushstring(L,"const ");
            lua_insert(L,-2);
            lua_concat(L,2);
            r = lua_isstring(L,-1) && strcmp(lua_tostring(L,-1),type)==0;
        }
    }
    lua_pop(L, 1);
    return r;
}

void lua_pushjson(lua_State *L, const mjson::Node &value)
{
    if(value.isNull())
    {
        lua_pushnil(L);
    }
    else if(value.isNumber())
    {
        lua_pushnumber(L, value.asFloat());
    }
    else if(value.isString())
    {
        lua_pushlstring(L, value.asCString(), value.size());
    }
    else if(value.isBool())
    {
        lua_pushboolean(L, value.asBool());
    }
    else if(value.isArray())
    {
        mjson::Array *array = value.rawArray();
        
        lua_newtable(L);
        int index = 1;
        for(const mjson::Node &node : *array)
        {
            lua_pushjson(L, node);
            lua_rawseti(L, -2, index++);
        }
    }
    else if(value.isDict())
    {
        mjson::Dict *dict = value.rawDict();
        
        lua_newtable(L);
        for(const auto &pair : *dict)
        {
            lua_pushlstring(L, pair.key.asCString(), pair.key.size());
            lua_pushjson(L, pair.value);
            lua_rawset(L, -3);
        }
    }
    else
    {
        CCAssert(false, "Shouldn't reach here.");
        lua_pushnil(L);
    }
}

void lua_pushcppstring(lua_State *L, const std::string &value)
{
    lua_pushlstring(L, value.c_str(), value.size());
}

void lua_setfield_bool(lua_State *L, const char *name, bool value)
{
    lua_pushboolean(L, value ? 1 : 0);
    lua_setfield(L, -2, name);
}

void lua_setfield_string(lua_State *L, const char *name, const char *value)
{
    lua_pushstring(L, value);
    lua_setfield(L, -2, name);
}

void lua_setfield_string(lua_State *L, const char *name, const std::string &value)
{
    lua_pushlstring(L, value.c_str(), value.size());
    lua_setfield(L, -2, name);
}


LuaStackChecker::LuaStackChecker(lua_State *L, const char *file, const char *func, int line)
    : state_(L)
    , file_(file)
    , func_(func)
    , line_(line)
{
    top_ = lua_gettop(L);
}

LuaStackChecker::~LuaStackChecker()
{
    int top = lua_gettop(state_);
    if(top != top_)
    {
        CCLOGERROR("The lua stack error at:\nfile: %s\nfunc: %s\nline: %d\norigin top(%d) vs new top(%d)",
                  file_, func_, line_, top_, top);
        CCAssert(0, "LuaStackChecker");
    }
}
