#ifndef CSBANIMATIONLOADER_H
#define CSBANIMATIONLOADER_H

#include "node_loader.h"

class PIP_DLL CSBAnimationLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CSBAnimationLoader)

    CSBAnimationLoader();

    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // CSBANIMATIONLOADER_H
