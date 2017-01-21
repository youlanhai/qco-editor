#include "text_loader.h"
#include "../json_helper.h"
#include <ui/UIText.h>

#ifdef BUILD_EDITOR
#include "editor/framework.h"
#include "editor/document.h"
#endif

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::Text, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(fontSize, setFontSize, float)
DECLARE_SETTER(fontColor, setTextColor, cocos2d::Color4B)
DECLARE_SETTER(touchScaleEnable, setTouchScaleChangeEnabled, bool)
DECLARE_SETTER(enableShadow, enableShadow, cocos2d::Color4B)
DECLARE_SETTER(enableOutline, enableOutline, cocos2d::Color4B)
DECLARE_SETTER(enableGlow, enableGlow, cocos2d::Color4B)
DECLARE_SETTER(textAreaSize, setTextAreaSize, cocos2d::Size)

#undef DECLARE_LOADER

DEFINE_PROPERTY_SETTER(cocos2d::ui::Text, text)
{
    node->setString(json2text(value));
}

DEFINE_PROPERTY_SETTER(cocos2d::ui::Text, fontName)
{
    node->setFontName(json2font(value));
}

void node_set_hAlignment(cocos2d::ui::Text *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->setTextHorizontalAlignment((cocos2d::TextHAlignment)type);
}

void node_set_vAlignment(cocos2d::ui::Text *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->setTextVerticalAlignment((cocos2d::TextVAlignment)type);
}

void node_set_disableEffect(cocos2d::ui::Text *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->disableEffect((cocos2d::LabelEffect)type);
}


UITextLoader::UITextLoader()
{
    REGISTER_PROPERTY_SETTER(text);
    REGISTER_PROPERTY_SETTER(fontName);
    REGISTER_PROPERTY_SETTER(fontSize);
    REGISTER_PROPERTY_SETTER(fontColor);
    REGISTER_PROPERTY_SETTER(hAlignment);
    REGISTER_PROPERTY_SETTER(vAlignment);
    REGISTER_PROPERTY_SETTER(touchScaleEnable);
    REGISTER_PROPERTY_SETTER(enableShadow);
    REGISTER_PROPERTY_SETTER(enableOutline);
    REGISTER_PROPERTY_SETTER(enableGlow);
    REGISTER_PROPERTY_SETTER(disableEffect);
    REGISTER_PROPERTY_SETTER(textAreaSize);
}

ObjectType* UITextLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::Text::create();
}
