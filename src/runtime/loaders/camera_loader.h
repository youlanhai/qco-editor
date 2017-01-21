#ifndef CCCAMERALOADER_H
#define CCCAMERALOADER_H

#include "node_loader.h"

class PIP_DLL CCCameraLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CCCameraLoader)

    CCCameraLoader();

    virtual ObjectType* createObject(const JsonHandle &config) override;
};


#endif // CCCAMERALOADER_H
