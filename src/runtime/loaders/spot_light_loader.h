#ifndef CCSPOTLIGHTLOADER_H
#define CCSPOTLIGHTLOADER_H

#include "light_loader.h"

class PIP_DLL CCSpotLightLoader : public CCBaseLightLoader
{
public:
    DECLARE_LOADER(CCSpotLightLoader)

    CCSpotLightLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};

#endif // CCSPOTLIGHTLOADER_H
