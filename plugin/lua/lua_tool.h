#ifndef LUA_TOOL_H
#define LUA_TOOL_H

#include <platform/CCPlatformMacros.h>
#include <QVariant>

struct lua_State;

namespace Editor
{
    QVariant lua2qvariant(lua_State *L, int idx);

    QString lua2qstring(lua_State *L, int idx);

    std::string lua2stdstring(lua_State *L, int idx);

    bool lua2qvlist(QVariantList &out, lua_State *L, int idx);

    bool lua2qvmap(QVariantMap &out, lua_State *L, int idx);

    QVariant lua2qvariant(lua_State *L, int idx);
}

#endif // LUA_TOOL_H
