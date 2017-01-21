#include "scrollview_loader.h"
#include "../json_helper.h"
#include <ui/UIScrollView.h>


#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::ScrollView, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(innerContainerSize, setInnerContainerSize, cocos2d::Size)
DECLARE_SETTER(innerContainerPosition, setInnerContainerPosition, cocos2d::Vec2)
DECLARE_SETTER(bounceEnabled, setBounceEnabled, bool)
DECLARE_SETTER(inertiaScrollEnabled, setInertiaScrollEnabled, bool)
DECLARE_SETTER(scrollBarEnabled, setScrollBarEnabled, bool)
DECLARE_SETTER(scrollBarPositionFromCorner, setScrollBarPositionFromCorner, cocos2d::Vec2)
DECLARE_SETTER(scrollBarPositionFromCornerForVertical, setScrollBarPositionFromCornerForVertical, cocos2d::Vec2)
DECLARE_SETTER(scrollBarPositionFromCornerForHorizontal, setScrollBarPositionFromCornerForHorizontal, cocos2d::Vec2)
DECLARE_SETTER(scrollBarWidth, setScrollBarWidth, float)
DECLARE_SETTER(scrollBarAutoHideEnabled, setScrollBarAutoHideEnabled, bool)
DECLARE_SETTER(scrollBarAutoHideTime, setScrollBarAutoHideTime, float)

#undef DECLARE_LOADER

static void node_set_direction(cocos2d::ui::ScrollView *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setDirection((cocos2d::ui::ScrollView::Direction)v);
}

static void node_set_scrollBarColor(cocos2d::ui::ScrollView *node, const JsonHandle &value, const JsonHandle &config)
{
    cocos2d::Color4B cr;
    value >> cr;
    node->setScrollBarColor(cocos2d::Color3B(cr));
    node->setScrollBarOpacity(cr.a);
}

UIScrollViewLoader::UIScrollViewLoader()
{
    REGISTER_PROPERTY_SETTER(direction);
    REGISTER_PROPERTY_SETTER(innerContainerSize);
    REGISTER_PROPERTY_SETTER(innerContainerPosition);
    REGISTER_PROPERTY_SETTER(bounceEnabled);
    REGISTER_PROPERTY_SETTER(inertiaScrollEnabled);
    REGISTER_PROPERTY_SETTER(scrollBarEnabled);
    REGISTER_PROPERTY_SETTER(scrollBarPositionFromCorner);
    REGISTER_PROPERTY_SETTER(scrollBarPositionFromCornerForVertical);
    REGISTER_PROPERTY_SETTER(scrollBarPositionFromCornerForHorizontal);
    REGISTER_PROPERTY_SETTER(scrollBarWidth);
    REGISTER_PROPERTY_SETTER(scrollBarColor);
    REGISTER_PROPERTY_SETTER(scrollBarAutoHideEnabled);
    REGISTER_PROPERTY_SETTER(scrollBarAutoHideTime);
}

ObjectType* UIScrollViewLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::ScrollView::create();
}
