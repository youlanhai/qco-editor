#ifndef UIBUTTONLOADER_H
#define UIBUTTONLOADER_H

#include "widget_loader.h"

class PIP_DLL UIButtonLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UIButtonLoader)

    UIButtonLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UIBUTTONLOADER_H
