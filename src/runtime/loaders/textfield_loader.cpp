#include "textfield_loader.h"
#include "../json_helper.h"
#include <ui/UITextField.h>


#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::TextField, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(maxLengthEnabled, setMaxLengthEnabled, bool)
DECLARE_SETTER(maxLength, setMaxLength, int)
DECLARE_SETTER(passwordEnabled, setPasswordEnabled, bool)
DECLARE_SETTER(placeHolderColor, setPlaceHolderColor, cocos2d::Color4B)
DECLARE_SETTER(textColor, setTextColor, cocos2d::Color4B)
DECLARE_SETTER(fontSize, setFontSize, int)

#undef DECLARE_LOADER

DEFINE_PROPERTY_SETTER(cocos2d::ui::TextField, text)
{
    node->setString(json2text(value));
}

DEFINE_PROPERTY_SETTER(cocos2d::ui::TextField, placeHolder)
{
    node->setPlaceHolder(json2text(value));
}

DEFINE_PROPERTY_SETTER(cocos2d::ui::TextField, fontName)
{
    node->setFontName(json2font(value));
}

UITextFieldLoader::UITextFieldLoader()
{
    REGISTER_PROPERTY_SETTER(maxLengthEnabled);
    REGISTER_PROPERTY_SETTER(maxLength);
    REGISTER_PROPERTY_SETTER(passwordEnabled);
    REGISTER_PROPERTY_SETTER(placeHolder);
    REGISTER_PROPERTY_SETTER(placeHolderColor);
    REGISTER_PROPERTY_SETTER(textColor);
    REGISTER_PROPERTY_SETTER(fontSize);
    REGISTER_PROPERTY_SETTER(fontName);
    REGISTER_PROPERTY_SETTER(text);
}

ObjectType* UITextFieldLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::TextField::create();
}
