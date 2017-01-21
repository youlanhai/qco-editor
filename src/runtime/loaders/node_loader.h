#ifndef __Editor__CCNodeLoader__
#define __Editor__CCNodeLoader__

#include "../object_loader.h"

class PIP_DLL CCNodeLoader : public ObjectLoader
{
public:
    DECLARE_LOADER(CCNodeLoader)
    
    CCNodeLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif /* defined(__Editor__CCNodeLoader__) */
