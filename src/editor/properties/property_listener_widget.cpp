#include "property_listener_widget.h"
#include "editor/properties/property_param.h"

#include "editor/framework.h"
#include "editor/menu_loader.h"
#include "editor/inspector.h"

#include <ui/UIWidget.h>
#include <ui/UILayoutComponent.h>
#include <ui/UIHelper.h>
#include <qtproperty.h>

namespace Editor
{
    static cocos2d::ui::LayoutComponent* getLayoutComponent(cocos2d::Node *node)
    {
        return dynamic_cast<cocos2d::ui::LayoutComponent*>(node->getComponent(__LAYOUT_COMPONENT_NAME));
    }

    static cocos2d::Vec4 getLayoutMargin(cocos2d::ui::LayoutComponent *component)
    {
        cocos2d::Vec4 margin;
        margin.x = component->getLeftMargin();
        margin.y = component->getRightMargin();
        margin.z = component->getTopMargin();
        margin.w = component->getBottomMargin();
        return margin;
    }

    static void resetNodeMargin(cocos2d::Node *widget)
    {
        cocos2d::ui::LayoutComponent *layout = getLayoutComponent(widget);
        if(nullptr == layout)
        {
            return;
        }

        cocos2d::Node *parent = widget->getParent();
        if (!parent)
        {
            return;
        }

        float left = widget->getPositionX() - widget->getContentSize().width * widget->getAnchorPoint().x;
        float right = parent->getContentSize().width - widget->getContentSize().width - left;
        float bottom = widget->getPositionY() - widget->getContentSize().height * widget->getAnchorPoint().y;
        float top = parent->getContentSize().height - widget->getContentSize().height - bottom;

        layout->setLeftMargin(left);
        layout->setRightMargin(right);
        layout->setTopMargin(top);
        layout->setBottomMargin(bottom);
    }

    static void refreshLayoutInfo(cocos2d::Node *node)
    {
        cocos2d::ui::LayoutComponent *layout = getLayoutComponent(node);
        if(nullptr == layout)
        {
            return;
        }

        JsonValue val;
        val = jsonFrom(node->getPosition3D());
        Framework::instance()->emitNodePropertyChange(node, "position", val);

        val = jsonFrom(node->getContentSize());
        Framework::instance()->emitNodePropertyChange(node, "size", val);

        resetNodeMargin(node);
        val = jsonFrom(getLayoutMargin(layout));
        Framework::instance()->emitComponentPropertyChange(layout, "margin", val);

        val = jsonFrom(layout->getPercentContentSize());
        Framework::instance()->emitComponentPropertyChange(layout, "percentSize", val);
    }

    static void refreshChildren(cocos2d::Node *parent)
    {
        for (const auto& node: parent->getChildren())
        {
            cocos2d::ui::LayoutComponent *layout = getLayoutComponent(node);
            if(nullptr == layout)
            {
                continue;
            }

            refreshLayoutInfo(node);
            refreshChildren(node);
        }
    }

    void PropertyListenerWidgetPosition::onPropertyChange(PropertyParam &param)
    {
        cocos2d::ui::Widget *p = dynamic_cast<cocos2d::ui::Widget*>(param.object);
        if(nullptr == p)
        {
            return;
        }

        cocos2d::ui::LayoutComponent *component = getLayoutComponent(p);
        if(component != nullptr)
        {
            cocos2d::Vec2 pos;
            param.value >> pos;

            component->setPosition(pos);

            JsonValue val = jsonFrom(getLayoutMargin(component));
            Framework::instance()->emitComponentPropertyChange(component, "margin", val);

            val = jsonFrom(cocos2d::Vec2(component->getPositionPercentX(), component->getPositionPercentY()));
            Framework::instance()->emitComponentPropertyChange(component, "percentPosition", val);
        }
    }

    void PropertyListenerWidgetSize::onPropertyChange(PropertyParam &param)
    {
        cocos2d::ui::Widget *p = dynamic_cast<cocos2d::ui::Widget*>(param.object);
        if(nullptr == p)
        {
            return;
        }

        cocos2d::ui::LayoutComponent *component = getLayoutComponent(p);
        if(component != nullptr)
        {
            cocos2d::Size size;
            param.value >> size;

            component->setSize(size);

            refreshLayoutInfo(p);
        }

        cocos2d::ui::Helper::doLayout(p);
        refreshChildren(p);
    }

    void PropertyListenerWidgetLayout::onPropertyChange(PropertyParam &param)
    {
        cocos2d::ui::Widget *p = dynamic_cast<cocos2d::ui::Widget*>(param.object);
        if(nullptr == p || p != Framework::instance()->getTargetNode())
        {
            return;
        }

        bool enable;
        param.value >> enable;

        cocos2d::ui::LayoutComponent *component = getLayoutComponent(p);
        if(enable == (component != nullptr))
        {
            return;
        }

        if(enable)
        {
            Framework::instance()->componentMenu_->emitCreateSignal("LayoutComponent");
        }
        else
        {
            // don't remove component, the outer function may be using this property.
        }
    }
}
