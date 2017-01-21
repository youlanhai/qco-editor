#include "richtext_loader.h"
#include "../json_helper.h"
#include <ui/UIRichText.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::RichText, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(verticalSpace, setVerticalSpace, float)

#undef DECLARE_LOADER

UIRichTextLoader::UIRichTextLoader()
{
    REGISTER_PROPERTY_SETTER(verticalSpace);
}

ObjectType* UIRichTextLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::RichText::create();
}
