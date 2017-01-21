#ifndef UILOADINGBARLOADER_H
#define UILOADINGBARLOADER_H

#include "widget_loader.h"

class PIP_DLL UILoadingBarLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UILoadingBarLoader)

    UILoadingBarLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UILOADINGBARLOADER_H
