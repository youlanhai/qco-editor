#ifndef UIWIDGETLOADER_H
#define UIWIDGETLOADER_H

#include "node_loader.h"


class PIP_DLL UIWidgetLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(UIWidgetLoader)

    UIWidgetLoader();

    virtual ObjectType* createObject(const JsonHandle &config) override;
    virtual void postLoaded(ObjectType *p, const JsonHandle &config) override;
};

#endif // UIWIDGETLOADER_H
