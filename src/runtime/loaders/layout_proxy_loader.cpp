#include "layout_proxy_loader.h"

#include "../extend/layout_proxy.h"
#include "../json_helper.h"


DECLARE_PROPERTY_SETTER(pip::LayoutProxy, layout, setLayout, ResourceHolder)

LayoutProxyLoader::LayoutProxyLoader()
{
    REGISTER_PROPERTY_SETTER(layout);
}

ObjectType* LayoutProxyLoader::createObject(const JsonHandle &config)
{
    return pip::LayoutProxy::create();
}

