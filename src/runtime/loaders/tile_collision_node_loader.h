#ifndef TILE_COLLISION_COMPONENT_LOADER_H
#define TILE_COLLISION_COMPONENT_LOADER_H

#include "node_loader.h"

class PIP_DLL TileCollisionNodeLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(TileCollisionNodeLoader)

    TileCollisionNodeLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // TILE_COLLISION_COMPONENT_LOADER_H
