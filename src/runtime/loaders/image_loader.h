#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "widget_loader.h"

class PIP_DLL UIImageLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UIImageLoader)
    UIImageLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // IMAGE_LOADER_H
