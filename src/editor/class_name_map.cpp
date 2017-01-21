#include "class_name_map.h"
#include "editor/properties/property_group.h"
#include "extend/editor_sprite3d.h"
#include "extend/editor_camera.h"
#include "runtime/extend/z_button.h"
#include "runtime/extend/z_textatlas.h"
#include "runtime/extend/csb_animation.h"
#include "runtime/extend/layout_proxy.h"

#include <cocos2d.h>

#include <ui/CocosGUI.h>

namespace Editor
{
    void registerClassNameMap(PropertyGroupMgr * pm)
    {
#define REGISTER_UI_NAME(CLASS, NAME) \
    pm->registerUIName(typeid(CLASS).name(), NAME)
  
        REGISTER_UI_NAME(cocos2d::Node, "Node");
        REGISTER_UI_NAME(cocos2d::Scene, "Scene");
        REGISTER_UI_NAME(cocos2d::Sprite, "Sprite");
        REGISTER_UI_NAME(cocos2d::Sprite3D, "Sprite3D");
        REGISTER_UI_NAME(EditorSprite3D, "Sprite3D");
        REGISTER_UI_NAME(cocos2d::DirectionLight, "DirectionLight");
        REGISTER_UI_NAME(cocos2d::PointLight, "PointLight");
        REGISTER_UI_NAME(cocos2d::SpotLight, "SpotLight");
        REGISTER_UI_NAME(cocos2d::AmbientLight, "AmbientLight");
        REGISTER_UI_NAME(cocos2d::Camera, "Camera");
        REGISTER_UI_NAME(EditorCamera, "Camera");
        REGISTER_UI_NAME(pip::CSBAnimation, "CSBAnimation");
        REGISTER_UI_NAME(pip::ZButton, "Button");
        REGISTER_UI_NAME(pip::ZTextAtlas, "TextAtlas");
        REGISTER_UI_NAME(pip::LayoutProxy, "LayoutProxy");

        REGISTER_UI_NAME(cocos2d::Layer, "Layer");
        REGISTER_UI_NAME(cocos2d::LayerColor, "LayerColor");
        
        REGISTER_UI_NAME(cocos2d::ui::Widget, "Widget");
        REGISTER_UI_NAME(cocos2d::ui::Button, "Button");
        REGISTER_UI_NAME(cocos2d::ui::Layout, "Layout");
        REGISTER_UI_NAME(cocos2d::ui::ScrollView, "ScrollView");
        REGISTER_UI_NAME(cocos2d::ui::PageView, "PageView");
        REGISTER_UI_NAME(cocos2d::ui::ListView, "ListView");
        REGISTER_UI_NAME(cocos2d::ui::ImageView, "Image");
        REGISTER_UI_NAME(cocos2d::ui::Text, "Text");
        REGISTER_UI_NAME(cocos2d::ui::RichText, "RichText");
        REGISTER_UI_NAME(cocos2d::ui::TextBMFont, "TextBMFont");
        REGISTER_UI_NAME(cocos2d::ui::TextAtlas, "TextAtlas");
        REGISTER_UI_NAME(cocos2d::ui::TextField, "TextField");
        REGISTER_UI_NAME(cocos2d::ui::EditBox, "EditBox");
        REGISTER_UI_NAME(cocos2d::ui::CheckBox, "CheckBox");
        REGISTER_UI_NAME(cocos2d::ui::Scale9Sprite, "Scale9Sprite");
        REGISTER_UI_NAME(cocos2d::ui::EditBox, "EditBox");
        REGISTER_UI_NAME(cocos2d::ui::Slider, "Slider");
        REGISTER_UI_NAME(cocos2d::ui::LoadingBar, "LoadingBar");

#undef REGISTER_UI_NAME
    }
}
