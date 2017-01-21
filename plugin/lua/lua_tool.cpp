#include "lua_tool.h"
#include "editor/tools/log_tool.h"
#include "plugin-lua/lua_helper.h"

#include <base/ccMacros.h>

extern "C" {
#include <lua.h>
}

namespace Editor
{

    QVariant lua2qvariant(lua_State *L, int idx);

    QString lua2qstring(lua_State *L, int idx)
    {
        CCAssert(lua_isstring(L, idx), "lua2qstring");

        size_t size;
        const char *str = lua_tolstring(L, idx, &size);
        return QString::fromUtf8(str, size);
    }

    std::string lua2stdstring(lua_State *L, int idx)
    {
        CCAssert(lua_isstring(L, idx), "lua2qstring");

        size_t size;
        const char *str = lua_tolstring(L, idx, &size);
        return std::string(str, size);
    }

    bool lua2qvlist(QVariantList &out, lua_State *L, int idx)
    {
        LUA_STACK_CHECKER(L);

        QVariantList list;
        bool isArray = true;
        int index = 1;

        lua_pushvalue(L, idx); // duplicate the table on top
        lua_pushnil(L); // push a nil key
        while(lua_next(L, -2) != 0)
        {
            if(lua_tonumber(L, -2) == index++)
            {
                QVariant val = lua2qvariant(L, -1);
                list.append(val);
                lua_pop(L, 1); // pop value only.
            }
            else
            {
                lua_pop(L, 2); // pop value and key.
                isArray = false;
                break;
            }
        }
        lua_pop(L, 1); // pop the table.

        if(isArray)
        {
            out.swap(list);
        }
        return isArray;
    }

    bool lua2qvmap(QVariantMap &out, lua_State *L, int idx)
    {
        LUA_STACK_CHECKER(L);

        QVariantMap map;
        bool isMap = true;

        lua_pushvalue(L, idx); // duplicate the table on top.
        lua_pushnil(L);
        while(lua_next(L, -2) != 0)
        {
            if(lua_isstring(L, -2))
            {
                QString key = lua2qstring(L, -2);
                QVariant val = lua2qvariant(L, -1);
                map[key] = val;
                lua_pop(L, 1); // pop value only
            }
            else
            {
                lua_pop(L, 2); // pop key and value
                isMap = false;
                break;
            }
        }
        lua_pop(L, 1); // pop the table

        if(isMap)
        {
            out.swap(map);
        }
        return isMap;
    }


    QVariant lua2qvariant(lua_State *L, int idx)
    {
        LUA_STACK_CHECKER(L);

        int type = lua_type(L, idx);
        switch (type)
        {
        case LUA_TBOOLEAN:
            return QVariant((bool)lua_toboolean(L, idx));
        case LUA_TNUMBER:
            return QVariant(lua_tonumber(L, idx));
        case LUA_TSTRING:
            return lua2qstring(L, idx);
        case LUA_TTABLE:
        {
            if(lua_objlen(L, idx) == 0)
            {
                return QVariantMap();
            }

            // quick check, wheather the table is an array.
            lua_rawgeti(L, idx, 1);
            bool isArray = lua_isnumber(L, -1) != 0; // treated as an array.
            lua_pop(L, 1);

            if(isArray)
            {
                QVariantList list;
                if(lua2qvlist(list, L, idx))
                {
                    return list;
                }
            }

            QVariantMap map;
            lua2qvmap(map, L, idx);
            return map;
        }
        default:
            return QVariant();
        }
    }

} // end namespace editor
