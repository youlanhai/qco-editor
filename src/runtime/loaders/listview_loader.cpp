#include "listview_loader.h"
#include "../json_helper.h"
#include <ui/UIListView.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::ListView, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(itemsMargin, setItemsMargin, float)

#undef DECLARE_LOADER

static void node_set_gravity(cocos2d::ui::ListView *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setGravity((cocos2d::ui::ListView::Gravity)v);
}

UIListViewLoader::UIListViewLoader()
{
    REGISTER_PROPERTY_SETTER(gravity);
    REGISTER_PROPERTY_SETTER(itemsMargin);
}

ObjectType* UIListViewLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::ListView::create();
}
