#ifndef TEXT_LOADER_H
#define TEXT_LOADER_H

#include "widget_loader.h"

class PIP_DLL UITextLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UITextLoader)
    UITextLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // TEXT_LOADER_H
