#ifndef UITEXTATLASLOADER_H
#define UITEXTATLASLOADER_H

#include "widget_loader.h"

class PIP_DLL UITextAtlasLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UITextAtlasLoader)

    UITextAtlasLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UITEXTATLASLOADER_H
