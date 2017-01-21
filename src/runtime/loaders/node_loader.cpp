//
//  node_loader.cpp
//  Editor
//
//  Created by C218-pc on 15/6/26.
//
//

#include "node_loader.h"
#include "../json_helper.h"
#include <2d/CCNode.h>
#include <2d/CCCamera.h>

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::Node, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER(name, setName, std::string)
DECLARE_SETTER(tag, setTag, int)
DECLARE_SETTER(order, setLocalZOrder, int)
DECLARE_SETTER(position, setPosition3D, cocos2d::Vec3)
DECLARE_SETTER(rotation, setRotation3D, cocos2d::Vec3)
DECLARE_SETTER(size, setContentSize, cocos2d::Size)
DECLARE_SETTER(anchor, setAnchorPoint, cocos2d::Vec2)
DECLARE_SETTER(ignoreAnchor, ignoreAnchorPointForPosition, bool)
DECLARE_SETTER(visible, setVisible, bool)

#undef DECLARE_SETTER

static void node_set_skew(cocos2d::Node *node, const JsonHandle &value, const JsonHandle &config)
{
    cocos2d::Vec2 v;
    value >> v;
    node->setSkewX(v.x);
    node->setSkewY(v.y);
}

static void node_set_scale(cocos2d::Node *node, const JsonHandle &value, const JsonHandle &config)
{
    cocos2d::Vec3 v;
    value >> v;
    node->setScaleX(v.x);
    node->setScaleY(v.y);
    node->setScaleZ(v.z);
}

static void node_set_color(cocos2d::Node *node, const JsonHandle &value, const JsonHandle &config)
{
    cocos2d::Color4B cr;
    value >> cr;
    node->setColor(cocos2d::Color3B(cr));
    node->setOpacity(cr.a);
}

static uint16_t index2cameraMask(uint16_t index)
{
    static cocos2d::CameraFlag Flags[] = {
        cocos2d::CameraFlag::DEFAULT,
        cocos2d::CameraFlag::USER1,
        cocos2d::CameraFlag::USER2,
        cocos2d::CameraFlag::USER3,
        cocos2d::CameraFlag::USER4,
        cocos2d::CameraFlag::USER5,
        cocos2d::CameraFlag::USER6,
        cocos2d::CameraFlag::USER7,
        cocos2d::CameraFlag::USER8
    };
    static const int NFlags = sizeof(Flags) / sizeof(cocos2d::CameraFlag);

    uint16_t mask = 0;
    for(uint16_t i = 0; i < NFlags; ++i)
    {
        if(index & (1 << i))
        {
            mask |= (uint16_t)Flags[i];
        }
    }
    return mask;
}

static void node_set_cameraMask(cocos2d::Node *node, const JsonHandle &value, const JsonHandle &config)
{
    uint16_t flag;
    value >> flag;

    flag = index2cameraMask(flag);

#ifdef BUILD_EDITOR
    flag |= (uint16_t)cocos2d::CameraFlag::USER8;
#endif

    node->setCameraMask(flag, false);
}

DECLARE_PROPERTY_GETTER(cocos2d::Node, name, getName, std::string)
DECLARE_PROPERTY_GETTER(cocos2d::Node, tag, getTag, int)
DECLARE_PROPERTY_GETTER(cocos2d::Node, order, getLocalZOrder, int)
DECLARE_PROPERTY_GETTER(cocos2d::Node, position, getPosition3D, cocos2d::Vec3)
DECLARE_PROPERTY_GETTER(cocos2d::Node, rotation, getRotation3D, cocos2d::Vec3)
DECLARE_PROPERTY_GETTER(cocos2d::Node, size, getContentSize, cocos2d::Size)
DECLARE_PROPERTY_GETTER(cocos2d::Node, anchor, getAnchorPoint, cocos2d::Vec2)
DECLARE_PROPERTY_GETTER(cocos2d::Node, ignoreAnchor, isIgnoreAnchorPointForPosition, bool)
DECLARE_PROPERTY_GETTER(cocos2d::Node, visible, isVisible, bool)
DEFINE_PROPERTY_GETTER(cocos2d::Node, sclae)
{
    cocos2d::Vec3 v(node->getScaleX(), node->getScaleY(), node->getScaleZ());
    return jsonFrom(v);
}

CCNodeLoader::CCNodeLoader()
{
    REGISTER_PROPERTY_SETTER(name);
    REGISTER_PROPERTY_SETTER(tag);
    REGISTER_PROPERTY_SETTER(order);
    REGISTER_PROPERTY_SETTER(position);
    REGISTER_PROPERTY_SETTER(scale);
    REGISTER_PROPERTY_SETTER(rotation);
    REGISTER_PROPERTY_SETTER(size);
    REGISTER_PROPERTY_SETTER(anchor);
    REGISTER_PROPERTY_SETTER(ignoreAnchor);
    REGISTER_PROPERTY_SETTER(visible);
    REGISTER_PROPERTY_SETTER(skew);
    REGISTER_PROPERTY_SETTER(color);
    REGISTER_PROPERTY_SETTER(cameraMask);


    REGISTER_PROPERTY_GETTER(name);
    REGISTER_PROPERTY_GETTER(tag);
    REGISTER_PROPERTY_GETTER(order);
    REGISTER_PROPERTY_GETTER(position);
    REGISTER_PROPERTY_GETTER(sclae);
    REGISTER_PROPERTY_GETTER(rotation);
    REGISTER_PROPERTY_GETTER(size);
    REGISTER_PROPERTY_GETTER(anchor);
    REGISTER_PROPERTY_GETTER(ignoreAnchor);
    REGISTER_PROPERTY_GETTER(visible);
}

ObjectType* CCNodeLoader::createObject(const JsonHandle &config)
{
    return cocos2d::Node::create();
}
