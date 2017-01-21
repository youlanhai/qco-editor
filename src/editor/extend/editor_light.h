#ifndef EDITORLIGHT_H
#define EDITORLIGHT_H

#include "2d/CClight.h"

//class EditorBaseLight : public cocos2d::BaseLight
//{
//public:

//CC_CONSTRUCTOR_ACCESS:
//    EditorBaseLight();
//    ~EditorBaseLight();
//};

class EditorDirectionLight : public cocos2d::DirectionLight
{
public:
    static EditorDirectionLight * create(const cocos2d::Vec3 &direction, const cocos2d::Color3B &color);

CC_CONSTRUCTOR_ACCESS:
    EditorDirectionLight();
    ~EditorDirectionLight();
};

class EditorPointLight : public cocos2d::PointLight
{
public:
    static EditorPointLight * create(const cocos2d::Vec3 &position, const cocos2d::Color3B &color, float range);

CC_CONSTRUCTOR_ACCESS:
    EditorPointLight();
    ~EditorPointLight();
};

class EditorSpotLight : public cocos2d::SpotLight
{
public:
    static EditorSpotLight * create(const cocos2d::Vec3 &direction, const cocos2d::Vec3 &position, const cocos2d::Color3B &color, float innerAngle, float outerAngle, float range);

CC_CONSTRUCTOR_ACCESS:
    EditorSpotLight();
    ~EditorSpotLight();
};

class EditorAmbientLight : public cocos2d::AmbientLight
{
public:
    static EditorAmbientLight * create(const cocos2d::Color3B &color);

CC_CONSTRUCTOR_ACCESS:
    EditorAmbientLight();
    ~EditorAmbientLight();
};

#endif // EDITORLIGHT_H
