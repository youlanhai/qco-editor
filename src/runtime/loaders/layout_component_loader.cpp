#include "layout_component_loader.h"

#include "../json_helper.h"
#include <ui/UILayoutComponent.h>

using cocos2d::ui::LayoutComponent;

DECLARE_PROPERTY_SETTER(LayoutComponent, percentOnly, setPercentOnlyEnabled, bool)
DECLARE_PROPERTY_SETTER(LayoutComponent, active, setActiveEnabled, bool)

DEFINE_PROPERTY_SETTER(LayoutComponent, percentSizeEnable)
{
    bool x, y;
    value[0u] >> x;
    value[1u] >> y;
    node->setPercentWidthEnabled(x);
    node->setPercentHeightEnabled(y);
}

DECLARE_PROPERTY_SETTER(LayoutComponent, percentSize, setPercentContentSize, cocos2d::Vec2)

DEFINE_PROPERTY_SETTER(LayoutComponent, percentPositionEnable)
{
    bool x, y;
    value[0u] >> x;
    value[1u] >> y;
    node->setPositionPercentXEnabled(x);
    node->setPositionPercentYEnabled(y);
}

DEFINE_PROPERTY_SETTER(LayoutComponent, percentPosition)
{
    cocos2d::Vec2 pos;
    value >> pos;
    node->setPositionPercentX(pos.x);
    node->setPositionPercentY(pos.y);
}

DEFINE_PROPERTY_SETTER(LayoutComponent, stretchSizeEnable)
{
    bool x, y;
    value[0u] >> x;
    value[1u] >> y;
    node->setStretchWidthEnabled(x);
    node->setStretchHeightEnabled(y);
}

DEFINE_PROPERTY_SETTER(LayoutComponent, horizontalEdge)
{
    int edge;
    value >> edge;
    node->setHorizontalEdge((LayoutComponent::HorizontalEdge)edge);
}

DEFINE_PROPERTY_SETTER(LayoutComponent, verticalEdge)
{
    int edge;
    value >> edge;
    node->setVerticalEdge((LayoutComponent::VerticalEdge)edge);
}

DEFINE_PROPERTY_SETTER(LayoutComponent, margin)
{
    float left, right, top, bottom;
    value[0u] >> left;
    value[1u] >> right;
    value[2u] >> top;
    value[3u] >> bottom;

    node->setLeftMargin(left);
    node->setRightMargin(right);
    node->setTopMargin(top);
    node->setBottomMargin(bottom);
}

DEFINE_PROPERTY_GETTER(LayoutComponent, margin)
{
    JsonValue value;

    mjson::Array *array = value.setArray();
    array->reserve(4);
    array->append(node->getLeftMargin());
    array->append(node->getRightMargin());
    array->append(node->getTopMargin());
    array->append(node->getBottomMargin());
    return value;
}

DEFINE_PROPERTY_GETTER(LayoutComponent, percentPosition)
{
    cocos2d::Vec2 pos(node->getPositionPercentX(), node->getPositionPercentY());
    return jsonFrom(pos);
}

DEFINE_PROPERTY_GETTER(LayoutComponent, percentSize)
{
    cocos2d::Vec2 pos(node->getPercentWidth(), node->getPercentHeight());
    return jsonFrom(pos);
}

LayoutComponentLoader::LayoutComponentLoader()
{
    REGISTER_PROPERTY_SETTER(percentPositionEnable);
    REGISTER_PROPERTY_SETTER(percentPosition);
    REGISTER_PROPERTY_SETTER(percentSizeEnable);
    REGISTER_PROPERTY_SETTER(percentSize);
    REGISTER_PROPERTY_SETTER(stretchSizeEnable);
    REGISTER_PROPERTY_SETTER(horizontalEdge);
    REGISTER_PROPERTY_SETTER(verticalEdge);
    REGISTER_PROPERTY_SETTER(margin);
    REGISTER_PROPERTY_SETTER(percentOnly);
    REGISTER_PROPERTY_SETTER(active);

    REGISTER_PROPERTY_GETTER(margin);
    REGISTER_PROPERTY_GETTER(percentPosition);
    REGISTER_PROPERTY_GETTER(percentSize);
}

ObjectType* LayoutComponentLoader::createObject(const JsonHandle &config)
{
    return LayoutComponent::create();
}
