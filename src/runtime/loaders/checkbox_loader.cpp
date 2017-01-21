#include "checkbox_loader.h"
#include <ui/UICheckBox.h>

#include "../json_helper.h"

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::CheckBox, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(selected, setSelected, bool)

#ifdef BUILD_EDITOR
DECLARE_SETTER(backGround, loadTextureBackGround, ResourceHolder)
DECLARE_SETTER(cross, loadTextureFrontCross, ResourceHolder)
#endif
#undef DECLARE_SETTER

UICheckBoxLoader::UICheckBoxLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(backGround);
    REGISTER_PROPERTY_SETTER(cross);
#endif
    REGISTER_PROPERTY_SETTER(selected);
}

ObjectType* UICheckBoxLoader::createObject(const JsonHandle &config)
{
    std::string backGround = json2string(config["backGround"]);
    RESOLVE_FILE_RESOURCE(backGround);
    std::string cross = json2string(config["cross"]);
    RESOLVE_FILE_RESOURCE(cross);

    if (backGround.empty() || cross.empty())
    {
        return cocos2d::ui::CheckBox::create();
    }
    return cocos2d::ui::CheckBox::create(backGround, cross);
}
