#ifndef LAYOUT_COMPONENT_LOADER_H
#define LAYOUT_COMPONENT_LOADER_H

#include "component_loader.h"

class PIP_DLL LayoutComponentLoader : public ComponentLoader
{
public:
    DECLARE_LOADER(LayoutComponentLoader)

    LayoutComponentLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // LAYOUT_COMPONENT_LOADER_H
