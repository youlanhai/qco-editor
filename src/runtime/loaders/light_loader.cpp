#include "light_loader.h"
#include "../json_helper.h"
#include <2d/CCLight.h>

/***********************    CCBaseLightLoader    ***********************/
#define DECLARE_SETTER_BASE(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::BaseLight, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER_BASE(intensity, setIntensity, float)
DECLARE_SETTER_BASE(enabled, setEnabled, bool)

#undef DECLARE_SETTER_BASE

static cocos2d::LightFlag index2lightFlag(int index)
{
    return (cocos2d::LightFlag)(1 << index);
}

static void node_set_lightFlag(cocos2d::DirectionLight *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setLightFlag(index2lightFlag(v));
}

CCBaseLightLoader::CCBaseLightLoader()
{
    REGISTER_PROPERTY_SETTER(lightFlag);
    REGISTER_PROPERTY_SETTER(intensity);
    REGISTER_PROPERTY_SETTER(enabled);
}


/***********************    CCDirectionLightLoader    ***********************/
#define DECLARE_SETTER_DIRECTION(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::DirectionLight, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER_DIRECTION(direction, setDirection, cocos2d::Vec3)

#undef DECLARE_SETTER_DIRECTION

CCDirectionLightLoader::CCDirectionLightLoader()
{
    REGISTER_PROPERTY_SETTER(direction);
}

ObjectType* CCDirectionLightLoader::createObject(const JsonHandle &config)
{
    cocos2d::Vec3 v = json2vec3(config["direction"], cocos2d::Vec3(-1.0f, 0.0f, 0.0f));
    cocos2d::Color4B cr = json2color4b(config["color"], cocos2d::Color4B(255, 255, 255, 255));

    return cocos2d::DirectionLight::create(v, cocos2d::Color3B(cr.r, cr.g, cr.b));
}

/***********************    CCPointLightLoader    ***********************/
#define DECLARE_SETTER_POINT(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::PointLight, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER_POINT(range, setRange, float)

#undef DECLARE_SETTER_POINT

CCPointLightLoader::CCPointLightLoader()
{
    REGISTER_PROPERTY_SETTER(range);
}

ObjectType* CCPointLightLoader::createObject(const JsonHandle &config)
{
    cocos2d::Vec3 vp = json2vec3(config["position"], cocos2d::Vec3(0.0f, 0.0f, 0.0f));
    cocos2d::Color4B cr = json2color4b(config["color"], cocos2d::Color4B(255, 255, 255, 255));
    float fr = json2float(config["range"], 10000.0);

    return cocos2d::PointLight::create(vp, cocos2d::Color3B(cr.r, cr.g, cr.b), fr);
}

/***********************    CCAmbientLightLoader    ***********************/
CCAmbientLightLoader::CCAmbientLightLoader()
{
}

ObjectType* CCAmbientLightLoader::createObject(const JsonHandle &config)
{
    cocos2d::Color4B cr = json2color4b(config["color"], cocos2d::Color4B(255, 255, 255, 255));

    return cocos2d::AmbientLight::create(cocos2d::Color3B(cr.r, cr.g, cr.b));
}
