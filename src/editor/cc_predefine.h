#ifndef CC_PREDEFINE_H
#define CC_PREDEFINE_H

#include <base/CCRefPtr.h>

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
    typedef cocos2d::RefPtr<cocos2d::Node> NodePtr;
    typedef cocos2d::RefPtr<cocos2d::Sprite> SpritePtr;
    typedef cocos2d::RefPtr<cocos2d::SpriteFrame> SpriteFramePtr;
    typedef cocos2d::RefPtr<cocos2d::Texture2D> Texture2DPtr;
    typedef cocos2d::RefPtr<cocos2d::Scene>     ScenePtr;
    typedef cocos2d::RefPtr<cocos2d::Camera>    CameraPtr;
    typedef cocos2d::RefPtr<cocos2d::Component> ComponentPtr;
}

#endif // CC_PREDEFINE_H

