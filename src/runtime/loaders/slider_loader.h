#ifndef UISLIDERLOADER_H
#define UISLIDERLOADER_H

#include "widget_loader.h"

class PIP_DLL UISliderLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UISliderLoader)

    UISliderLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UISLIDERLOADER_H
