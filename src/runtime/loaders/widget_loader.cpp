#include "widget_loader.h"
#include "../json_helper.h"
#include <ui/UIWidget.h>
#include <ui/UILayoutComponent.h>
#include <ui/UILayoutParameter.h>
#include <2d/CCNode.h>
#include <base/CCDirector.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::Widget, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(enabled, setEnabled, bool)
DECLARE_SETTER(touchEnabled, setTouchEnabled, bool)
DECLARE_SETTER(ignoreContentAdaptWithSize, ignoreContentAdaptWithSize, bool)
DECLARE_SETTER(layoutComponentEnable, setLayoutComponentEnabled, bool)

#undef DECLARE_SETTER

UIWidgetLoader::UIWidgetLoader()
{
    REGISTER_PROPERTY_SETTER(enabled);
    REGISTER_PROPERTY_SETTER(touchEnabled);
    REGISTER_PROPERTY_SETTER(ignoreContentAdaptWithSize);
    REGISTER_PROPERTY_SETTER(layoutComponentEnable);
}

ObjectType* UIWidgetLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::Widget::create();
}

void UIWidgetLoader::postLoaded(ObjectType *p, const JsonHandle &config)
{

}
