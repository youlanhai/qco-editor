#ifndef TEXTFIELDLOADER_H
#define TEXTFIELDLOADER_H

#include "widget_loader.h"

class PIP_DLL UITextFieldLoader : public UIWidgetLoader
{
public:
    DECLARE_LOADER(UITextFieldLoader)
    UITextFieldLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // TEXTFIELDLOADER_H
