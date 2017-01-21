#include "spot_light_loader.h"
#include "../json_helper.h"
#include <2d/CCLight.h>

/***********************    CCSpotLightLoader    ***********************/
#define DECLARE_SETTER_SPOT(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(cocos2d::SpotLight, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

DECLARE_SETTER_SPOT(direction, setDirection, cocos2d::Vec3)
DECLARE_SETTER_SPOT(innerAngle, setInnerAngle, float)
DECLARE_SETTER_SPOT(outerAngle, setOuterAngle, float)
DECLARE_SETTER_SPOT(range, setRange, float)

#undef DECLARE_SETTER_SPOT

CCSpotLightLoader::CCSpotLightLoader()
{
    REGISTER_PROPERTY_SETTER(direction);
    REGISTER_PROPERTY_SETTER(innerAngle);
    REGISTER_PROPERTY_SETTER(outerAngle);
    REGISTER_PROPERTY_SETTER(range);
}

ObjectType* CCSpotLightLoader::createObject(const JsonHandle &config)
{
    cocos2d::Vec3 vdir = json2vec3(config["direction"], cocos2d::Vec3(-1.0f, 0.0f, 0.0f));
    cocos2d::Vec3 vpos = json2vec3(config["position"], cocos2d::Vec3(0.0f, 0.0f, 0.0f));
    cocos2d::Color4B cr = json2color4b(config["color"], cocos2d::Color4B(255, 255, 255, 255));
    float finnerAngle = json2float(config["innerAngle"], 0.0);
    float fouterAngle = json2float(config["outerAngle"], 0.5);
    float frange = json2float(config["range"], 10000.0);

    return cocos2d::SpotLight::create(vdir, vpos, cocos2d::Color3B(cr.r, cr.g, cr.b), finnerAngle, fouterAngle, frange);
}

