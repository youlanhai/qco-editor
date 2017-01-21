#ifndef __Editor__CCSpriteLoader__
#define __Editor__CCSpriteLoader__

#include "node_loader.h"

class PIP_DLL CCSpriteLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CCSpriteLoader)

    CCSpriteLoader();
    
    virtual ObjectType* createObject(const JsonHandle & config) override;
};

#endif /* defined(__Editor__CCSpriteLoader__) */
