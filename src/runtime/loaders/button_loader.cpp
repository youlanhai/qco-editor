#include "button_loader.h"
#include "../json_helper.h"
#include "../extend/z_button.h"


#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(pip::ZButton, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(scale9enable, setScale9Enabled, bool)
DECLARE_SETTER(scale9rect, setCapInsets, cocos2d::Rect)
DECLARE_SETTER(pressActionEnable, setPressedActionEnabled, bool)
DECLARE_SETTER(pressZoomScale, setZoomScale, float)
DECLARE_SETTER(fontColor, setTitleColor, cocos2d::Color3B)
DECLARE_SETTER(fontSize, setTitleFontSize, float)

#undef DECLARE_SETTER

DEFINE_PROPERTY_SETTER(pip::ZButton, text)
{
    node->setTitleText(json2text(value));
}

DEFINE_PROPERTY_SETTER(pip::ZButton, fontName)
{
    node->setTitleFontName(json2font(value));
}

DEFINE_PROPERTY_SETTER(pip::ZButton, textImage)
{
    std::string filename;
    int style;
    value[0u] >> filename;
    value[1u] >> style;

    RESOLVE_FILE_RESOURCE(filename);
    node->setTextImage(filename, (pip::ZButton::ImageStyle)style);
}

DEFINE_PROPERTY_SETTER(pip::ZButton, customImage)
{
    std::string filename;
    int style;
    value[0u] >> filename;
    value[1u] >> style;

    RESOLVE_FILE_RESOURCE(filename);
    node->setCustomImage(filename, (pip::ZButton::ImageStyle)style);
}

DEFINE_PROPERTY_SETTER(pip::ZButton, alignment)
{
    int type;
    value >> type;
    node->setTitleAlignment((cocos2d::TextHAlignment)type);
}

DEFINE_PROPERTY_SETTER(pip::ZButton, image)
{
    std::string imageNormal, imagePressed, imageDisabled;
    value[0u] >> imageNormal;
    value[1u] >> imagePressed;
    value[1u] >> imageDisabled;

    RESOLVE_FILE_RESOURCE(imageNormal);
    RESOLVE_FILE_RESOURCE(imagePressed);
    RESOLVE_FILE_RESOURCE(imageDisabled);

    node->loadTextureNormal(imageNormal);
    node->loadTexturePressed(imagePressed);
    node->loadTextureDisabled(imageDisabled);
}

UIButtonLoader::UIButtonLoader()
{
    REGISTER_PROPERTY_SETTER(image);
    REGISTER_PROPERTY_SETTER(customImage);
    REGISTER_PROPERTY_SETTER(textImage);
    REGISTER_PROPERTY_SETTER(scale9enable);
    REGISTER_PROPERTY_SETTER(scale9rect);
    REGISTER_PROPERTY_SETTER(pressActionEnable);
    REGISTER_PROPERTY_SETTER(pressZoomScale);
    REGISTER_PROPERTY_SETTER(text);
    REGISTER_PROPERTY_SETTER(fontName);
    REGISTER_PROPERTY_SETTER(fontColor);
    REGISTER_PROPERTY_SETTER(fontSize);
    REGISTER_PROPERTY_SETTER(alignment);
}

ObjectType* UIButtonLoader::createObject(const JsonHandle &config)
{
    return  pip::ZButton::create();
}
