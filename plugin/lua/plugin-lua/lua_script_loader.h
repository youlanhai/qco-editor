#ifndef SCRIPTCOMPONENTLOADER_H
#define SCRIPTCOMPONENTLOADER_H

#include "runtime/loaders/component_loader.h"

class LuaScriptLoader : public ComponentLoader
{
public:
    DECLARE_LOADER(LuaScriptLoader)

    LuaScriptLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // SCRIPTCOMPONENTLOADER_H
