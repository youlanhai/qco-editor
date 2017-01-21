#include "../object_factory.h"

#include "node_loader.h"
#include "sprite_loader.h"
#include "sprite3dloader.h"
#include "light_loader.h"
#include "spot_light_loader.h"
#include "camera_loader.h"
#include "widget_loader.h"
#include "layout_loader.h"
#include "button_loader.h"
#include "text_loader.h"
#include "image_loader.h"
#include "textfield_loader.h"
#include "csb_animation_loader.h"
#include "scrollview_loader.h"
#include "pageview_loader.h"
#include "listview_loader.h"
#include "slider_loader.h"
#include "loadingbar_loader.h"
#include "checkbox_loader.h"
#include "richtext_loader.h"
#include "textbmfont_loader.h"
#include "textatlas_loader.h"
#include "physicsbody_loader.h"
#include "tile_collision_node_loader.h"
#include "layout_component_loader.h"
#include "layout_proxy_loader.h"
#include "particle_loader.h"

void registerLoaders(ObjectFactory *ld)
{
    ld->registerLoader("Node", new CCNodeLoader());
    ld->registerLoader("Sprite", new CCSpriteLoader());
    ld->registerLoader("Sprite3D", new CCSprite3DLoader());
    ld->registerLoader("BaseLight", new CCBaseLightLoader());
    ld->registerLoader("DirectionLight", new CCDirectionLightLoader());
    ld->registerLoader("PointLight", new CCPointLightLoader());
    ld->registerLoader("SpotLight", new CCSpotLightLoader());
    ld->registerLoader("AmbientLight", new CCAmbientLightLoader());
    ld->registerLoader("Camera", new CCCameraLoader());
    ld->registerLoader("Widget", new UIWidgetLoader());
    ld->registerLoader("Layout", new UILayoutLoader());
    ld->registerLoader("Button", new UIButtonLoader());
    ld->registerLoader("Text", new UITextLoader());
    ld->registerLoader("RichText", new UIRichTextLoader());
    ld->registerLoader("TextBMFont", new UITextBMFontLoader());
    ld->registerLoader("TextAtlas", new UITextAtlasLoader());
    ld->registerLoader("Image", new UIImageLoader());
    ld->registerLoader("TextField", new UITextFieldLoader());
    ld->registerLoader("ScrollView", new UIScrollViewLoader());
    ld->registerLoader("PageView", new UIPageViewLoader());
    ld->registerLoader("ListView", new UIListViewLoader());
    ld->registerLoader("Slider", new UISliderLoader());
    ld->registerLoader("LoadingBar", new UILoadingBarLoader());
    ld->registerLoader("CheckBox", new UICheckBoxLoader());
    ld->registerLoader("CSBAnimation", new CSBAnimationLoader());
    ld->registerLoader("Component", new ComponentLoader());
    ld->registerLoader("PhysicsBody", new PhysicsBodyLoader());
    ld->registerLoader("TileCollisionNode", new TileCollisionNodeLoader());
    ld->registerLoader("LayoutComponent", new LayoutComponentLoader());
    ld->registerLoader("LayoutProxy", new LayoutProxyLoader());
    ld->registerLoader("ParticleSystem", new ParticleSystemLoader());
}
