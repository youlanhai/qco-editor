#ifndef COMPONENTLOADER_H
#define COMPONENTLOADER_H

#include "../object_loader.h"

class PIP_DLL ComponentLoader : public ObjectLoader
{
public:
    DECLARE_LOADER(ComponentLoader)

    ComponentLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // COMPONENTLOADER_H
