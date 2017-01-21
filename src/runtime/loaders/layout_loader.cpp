#include "layout_loader.h"
#include <ui/UILayout.h>
#include "../json_helper.h"

using cocos2d::ui::Layout;

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::Layout, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(backgroundImage, setBackGroundImage, ResourceHolder)
DECLARE_SETTER(scale9enable, setBackGroundImageScale9Enabled, bool)
DECLARE_SETTER(scale9rect, setBackGroundImageCapInsets, cocos2d::Rect)
DECLARE_SETTER(clippingEnable, setClippingEnabled, bool)

#undef DECLARE_SETTER

void node_set_backgroundColorType(Layout *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->setBackGroundColorType((Layout::BackGroundColorType)type);
}

void node_set_backgroundColor(Layout *p, const JsonHandle &value, const JsonHandle &config)
{
    cocos2d::Color4B color;
    value >> color;
    p->setBackGroundColor(cocos2d::Color3B(color));
    p->setBackGroundColorOpacity(color.a);
}

void node_set_clippingType(Layout *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->setClippingType((Layout::ClippingType)type);
}

void node_set_layoutType(Layout *p, const JsonHandle &value, const JsonHandle &config)
{
    int type;
    value >> type;
    p->setLayoutType((Layout::Type)type);
}

UILayoutLoader::UILayoutLoader()
{
    REGISTER_PROPERTY_SETTER(backgroundImage);
    REGISTER_PROPERTY_SETTER(scale9enable);
    REGISTER_PROPERTY_SETTER(scale9rect);
    REGISTER_PROPERTY_SETTER(backgroundColorType);
    REGISTER_PROPERTY_SETTER(backgroundColor);
    REGISTER_PROPERTY_SETTER(clippingEnable);
    REGISTER_PROPERTY_SETTER(clippingType);
    REGISTER_PROPERTY_SETTER(layoutType);
}


ObjectType* UILayoutLoader::createObject(const JsonHandle &config)
{
    return Layout::create();
}
