#ifndef CCLIGHTLOADER_H
#define CCLIGHTLOADER_H

#include "node_loader.h"

class PIP_DLL CCBaseLightLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CCBaseLightLoader)

    CCBaseLightLoader();
};

class PIP_DLL CCDirectionLightLoader : public CCBaseLightLoader
{
public:
    DECLARE_LOADER(CCDirectionLightLoader)

    CCDirectionLightLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};

class PIP_DLL CCPointLightLoader : public CCBaseLightLoader
{
public:
    DECLARE_LOADER(CCPointLightLoader)

    CCPointLightLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};

class PIP_DLL CCAmbientLightLoader : public CCBaseLightLoader
{
public:
    DECLARE_LOADER(CCAmbientLightLoader)

    CCAmbientLightLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};

#endif // CCLIGHTLOADER_H
