#ifndef CC_PREDEFINE_H
#define CC_PREDEFINE_H

#include <platform/CCPlatformMacros.h>
#include "runtime/smartpointer.h"

NS_CC_BEGIN
class Node;
class Sprite;
class SpriteFrame;
class Texture2D;
class Scene;
class Camera;
class Component;
NS_CC_END

namespace Editor
{
    typedef SmartPointer<cocos2d::Node> NodePtr;
    typedef SmartPointer<cocos2d::Sprite> SpritePtr;
    typedef SmartPointer<cocos2d::SpriteFrame> SpriteFramePtr;
    typedef SmartPointer<cocos2d::Texture2D> Texture2DPtr;
    typedef SmartPointer<cocos2d::Scene>     ScenePtr;
    typedef SmartPointer<cocos2d::Camera>    CameraPtr;
    typedef SmartPointer<cocos2d::Component> ComponentPtr;
}

#endif // CC_PREDEFINE_H

