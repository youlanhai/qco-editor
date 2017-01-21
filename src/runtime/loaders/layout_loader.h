#ifndef LAYOUT_LOADER_H
#define LAYOUT_LOADER_H

#include "widget_loader.h"

class PIP_DLL UILayoutLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UILayoutLoader)

    UILayoutLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // LAYOUT_LOADER_H
