#ifndef LAYOUT_PROXY_LOADER_H
#define LAYOUT_PROXY_LOADER_H

#include "node_loader.h"

class PIP_DLL LayoutProxyLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(LayoutProxyLoader)

    LayoutProxyLoader();

    virtual ObjectType* createObject(const JsonHandle & config) override;
};


#endif // LAYOUT_PROXY_LOADER_H
