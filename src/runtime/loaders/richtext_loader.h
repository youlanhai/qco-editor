#ifndef UIRICHTEXTLOADER_H
#define UIRICHTEXTLOADER_H

#include "widget_loader.h"

class PIP_DLL UIRichTextLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UIRichTextLoader)
    UIRichTextLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UIRICHTEXTLOADER_H
