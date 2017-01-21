#ifndef RUNTIME_SPRITE_TOOL_H
#define RUNTIME_SPRITE_TOOL_H

#include "pip_config.h"
#include <platform/CCCommon.h>
#include <string>

NS_CC_BEGIN
class Texture2D;
class Sprite;
class SpriteFrame;
class Rect;

namespace ui
{
    class Scale9Sprite;
}
NS_CC_END

NS_PIP_BEGIN

PIP_DLL cocos2d::Texture2D * loadTexture(const std::string & filename);
PIP_DLL cocos2d::Texture2D * loadDefaultTexture();

PIP_DLL cocos2d::SpriteFrame * createSpriteFrame(const std::string & filename);
PIP_DLL cocos2d::SpriteFrame * createSpriteFrameWithRect(const std::string & filename, const cocos2d::Rect &rect);

PIP_DLL cocos2d::Sprite * createSprite(const std::string & filename);
PIP_DLL cocos2d::Sprite * createSpriteWithRect(const std::string &filename, const cocos2d::Rect &rect);

PIP_DLL cocos2d::ui::Scale9Sprite * createScale9Sprite(const std::string & filename);

NS_PIP_END


#endif /* defined(RUNTIME_SPRITE_TOOL_H) */
