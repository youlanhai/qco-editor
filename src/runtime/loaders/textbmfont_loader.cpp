#include "textbmfont_loader.h"

#include "../json_helper.h"
#include <ui/UITextBMFont.h>

DEFINE_PROPERTY_SETTER(cocos2d::ui::TextBMFont, fontName)
{
    node->setFntFile(json2font(value));
}

DEFINE_PROPERTY_SETTER(cocos2d::ui::TextBMFont, text)
{
    node->setString(json2text(value));
}

UITextBMFontLoader::UITextBMFontLoader()
{
    REGISTER_PROPERTY_SETTER(text);
    REGISTER_PROPERTY_SETTER(fontName);
}

ObjectType* UITextBMFontLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::TextBMFont::create();
}
