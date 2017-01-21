#include "pageview_loader.h"
#include "../json_helper.h"
#include <cocos2d.h>
#include <ui/UIPageView.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::PageView, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

#if COCOS2D_VERSION > 0x00030800
DECLARE_SETTER(curPageIndex, setCurrentPageIndex, int)
#else
DECLARE_SETTER(curPageIndex, setCurPageIndex, int)
#endif

DECLARE_SETTER(customScrollThreshold, setCustomScrollThreshold, float)
DECLARE_SETTER(usingCustomScrollThreshold, setUsingCustomScrollThreshold, bool)

#undef DECLARE_LOADER

static void node_set_direction(cocos2d::ui::PageView *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setDirection((cocos2d::ui::PageView::Direction)v);
}

UIPageViewLoader::UIPageViewLoader()
{
    REGISTER_PROPERTY_SETTER(direction);
    REGISTER_PROPERTY_SETTER(curPageIndex);
    REGISTER_PROPERTY_SETTER(customScrollThreshold);
    REGISTER_PROPERTY_SETTER(usingCustomScrollThreshold);
}

ObjectType* UIPageViewLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::PageView::create();
}
