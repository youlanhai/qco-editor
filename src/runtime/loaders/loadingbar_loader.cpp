#include "loadingbar_loader.h"

#include "../json_helper.h"

#include <ui/UILoadingBar.h>

#ifdef BUILD_EDITOR
DECLARE_PROPERTY_SETTER(cocos2d::ui::LoadingBar, texture, loadTexture, ResourceHolder)
DECLARE_PROPERTY_SETTER(cocos2d::ui::LoadingBar, percent, setPercent, float)
#endif

static void node_set_direction(cocos2d::ui::LoadingBar *node, const JsonHandle &value, const JsonHandle &config)
{
    int direction;
    value >> direction;
    node->setDirection(cocos2d::ui::LoadingBar::Direction(direction));
}

UILoadingBarLoader::UILoadingBarLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(texture);
    REGISTER_PROPERTY_SETTER(percent);
#endif
    REGISTER_PROPERTY_SETTER(direction);
}

ObjectType* UILoadingBarLoader::createObject(const JsonHandle &config)
{
    std::string texture = json2string(config["texture"]);
    RESOLVE_FILE_RESOURCE(texture);

    float percentage = json2float(config["percent"]);

    return cocos2d::ui::LoadingBar::create(texture, percentage);
}
