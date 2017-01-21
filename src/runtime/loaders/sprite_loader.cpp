#include "sprite_loader.h"

#include <2d/CCSprite.h>

#include "../json_helper.h"
#include "../sprite_tool.h"

#ifdef BUILD_EDITOR
DECLARE_PROPERTY_SETTER(cocos2d::Sprite, image, setTexture, ResourceHolder)
DECLARE_PROPERTY_SETTER(cocos2d::Sprite, imageRect, setTextureRect, cocos2d::Rect)
#endif

DECLARE_PROPERTY_SETTER(cocos2d::Sprite, blend, setBlendFunc, cocos2d::BlendFunc)

static void node_set_flip(cocos2d::Sprite *node, const JsonHandle &value, const JsonHandle &config)
{
    if(value.isArray() && value->size() >= 2)
    {
        bool flipX, flipY;
        value[0u] >> flipX;
        value[1u] >> flipY;

        node->setFlippedX(flipX);
        node->setFlippedY(flipY);
    }
}

CCSpriteLoader::CCSpriteLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(image);
    REGISTER_PROPERTY_SETTER(imageRect);
#endif

    REGISTER_PROPERTY_SETTER(blend);
    REGISTER_PROPERTY_SETTER(flip);
}

ObjectType* CCSpriteLoader::createObject(const JsonHandle &config)
{
    std::string image = json2string(config["image"]);
    if(!image.empty())
    {
        RESOLVE_FILE_RESOURCE(image);
        JsonHandle jrect = config["imageRect"];
        if(jrect.isArray())
        {
            cocos2d::Rect rc;
            jrect >> rc;

            return pip::createSpriteWithRect(image, rc);
        }

        return pip::createSprite(image);
    }
    return cocos2d::Sprite::create();
}

