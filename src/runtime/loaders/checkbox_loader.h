#ifndef UICHECKBOXLOADER_H
#define UICHECKBOXLOADER_H

#include "widget_loader.h"

class PIP_DLL UICheckBoxLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UICheckBoxLoader)

    UICheckBoxLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UICHECKBOXLOADER_H
