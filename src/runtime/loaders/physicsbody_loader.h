#ifndef PHYSICSBODY_LOADER_H
#define PHYSICSBODY_LOADER_H

#include "component_loader.h"

class PIP_DLL PhysicsBodyLoader : public ComponentLoader
{
public:
    DECLARE_LOADER(PhysicsBodyLoader)

    PhysicsBodyLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif //PHYSICSBODY_LOADER_H
