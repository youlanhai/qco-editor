#include "sprite_tool.h"

#include <2d/CCSprite.h>
#include <ui/UIScale9Sprite.h>
#include <renderer/CCTextureCache.h>
#include <platform/CCFileUtils.h>
#include <base/CCDirector.h>

#include "binary_reader.h"
#include "helper.h"
#include "string_tool.h"

NS_PIP_BEGIN

cocos2d::Texture2D * loadTexture(const std::string & filename)
{
    auto textureCache = cocos2d::Director::getInstance()->getTextureCache();
    return textureCache->addImage(filename);
}

cocos2d::Texture2D * loadDefaultTexture()
{
    static unsigned char cc_2x2_white_image[] = {
        // RGBA8888
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    const char * CC_2x2_WHITE_IMAGE_KEY = "cc_2x2_white_image";
    
    auto textureCache = cocos2d::Director::getInstance()->getTextureCache();
    
    // Gets the texture by key firstly.
    cocos2d::Texture2D * texture = textureCache->getTextureForKey(CC_2x2_WHITE_IMAGE_KEY);
    
    // If texture wasn't in cache, create it from RAW data.
    if (NULL == texture)
    {
        cocos2d::Image* image = new cocos2d::Image();
        bool isOK = image->initWithRawData(cc_2x2_white_image, sizeof(cc_2x2_white_image), 2, 2, 8);
        assert(isOK && "The 2x2 empty texture was created unsuccessfully.");
        
        texture = textureCache->addImage(image, CC_2x2_WHITE_IMAGE_KEY);
        CC_SAFE_RELEASE(image);
    }
    
    return texture;
}

cocos2d::SpriteFrame * createSpriteFrame(const std::string & filename)
{
    cocos2d::Texture2D * texture = loadTexture(filename);
    if(NULL == texture)
    {
        return NULL;
    }
    cocos2d::Rect rc(0, 0, texture->getContentSize().width, texture->getContentSize().height);
    return cocos2d::SpriteFrame::createWithTexture(texture, rc);
}

cocos2d::SpriteFrame * createSpriteFrameWithRect(const std::string & filename, const cocos2d::Rect &rect)
{
    cocos2d::Texture2D * texture = loadTexture(filename);
    if(NULL == texture)
    {
        return NULL;
    }
    return cocos2d::SpriteFrame::createWithTexture(texture, rect);
}

cocos2d::Sprite * createSprite(const std::string & filename)
{
    cocos2d::Texture2D * texture = loadTexture(filename);
    if(NULL == texture)
    {
        return NULL;
    }
    return cocos2d::Sprite::createWithTexture(texture);
}

cocos2d::Sprite * createSpriteWithRect(const std::string &filename, const cocos2d::Rect &rect)
{
    cocos2d::Texture2D * texture = loadTexture(filename);
    if(NULL == texture)
    {
        return NULL;
    }
    return cocos2d::Sprite::createWithTexture(texture, rect);
}

cocos2d::ui::Scale9Sprite * createScale9Sprite(const std::string & filename)
{
    cocos2d::Texture2D * texture = loadTexture(filename);
    if(NULL == texture)
    {
        return NULL;
    }
    
    cocos2d::Rect rc(0, 0, texture->getContentSize().width, texture->getContentSize().height);
    
    cocos2d::SpriteFrame * frame = cocos2d::SpriteFrame::createWithTexture(texture, rc);
    
    return cocos2d::ui::Scale9Sprite::createWithSpriteFrame(frame);
}

NS_PIP_END

