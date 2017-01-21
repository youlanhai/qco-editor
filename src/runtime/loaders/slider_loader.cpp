#include "slider_loader.h"

#include "../json_helper.h"

#include <ui/UISlider.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::Slider, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

#ifdef BUILD_EDITOR
DECLARE_SETTER(barTexture, loadBarTexture, ResourceHolder)
#endif

DECLARE_SETTER(maxPercent, setMaxPercent, int)

#undef DECLARE_SETTER

UISliderLoader::UISliderLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(barTexture);
#endif
    REGISTER_PROPERTY_SETTER(maxPercent);
}

ObjectType* UISliderLoader::createObject(const JsonHandle &config)
{
    std::string barTexture = json2string(config["barTexture"]);
    RESOLVE_FILE_RESOURCE(barTexture);

    return cocos2d::ui::Slider::create(barTexture, "");
}
