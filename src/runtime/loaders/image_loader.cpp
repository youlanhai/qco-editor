#include "image_loader.h"

#include <ui/UIImageView.h>

#include "../json_helper.h"

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::ui::ImageView, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(image, loadTexture, ResourceHolder)
DECLARE_SETTER(imageRect, setTextureRect, cocos2d::Rect)
DECLARE_SETTER(scale9enable, setScale9Enabled, bool)
DECLARE_SETTER(scale9rect, setCapInsets, cocos2d::Rect)

#undef DECLARE_LOADER

UIImageLoader::UIImageLoader()
{
    REGISTER_PROPERTY_SETTER(image);
    REGISTER_PROPERTY_SETTER(imageRect);
    REGISTER_PROPERTY_SETTER(scale9enable);
    REGISTER_PROPERTY_SETTER(scale9rect);
}

ObjectType* UIImageLoader::createObject(const JsonHandle &config)
{
    return cocos2d::ui::ImageView::create();
}
