#ifndef CCSPRITE3DLOADER_H
#define CCSPRITE3DLOADER_H

#include "node_loader.h"

class PIP_DLL CCSprite3DLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CCSprite3DLoader)

    CCSprite3DLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};

#endif // CCSPRITE3DLOADER_H
