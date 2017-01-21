#ifndef UITEXTBMFONT_LOADER_H
#define UITEXTBMFONT_LOADER_H

#include "widget_loader.h"

class PIP_DLL UITextBMFontLoader : public UIWidgetLoader
{
    public:
        DECLARE_LOADER(UITextBMFontLoader)
        UITextBMFontLoader();
        virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UITEXTBMFONT_LOADER_H
