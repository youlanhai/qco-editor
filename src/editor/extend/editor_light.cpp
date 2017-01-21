#include "editor_light.h"


//EditorBaseLight::EditorBaseLight()
//{
//}

//EditorBaseLight::~EditorBaseLight()
//{
//}

/***********    EditorDirectionLight    *********/
EditorDirectionLight::EditorDirectionLight()
{
}

EditorDirectionLight::~EditorDirectionLight()
{
}

EditorDirectionLight* EditorDirectionLight::create(const cocos2d::Vec3 &direction, const cocos2d::Color3B &color)
{
    EditorDirectionLight *dLight = new (std::nothrow) EditorDirectionLight();
    if (dLight)
    {
        dLight->setRotationFromDirection(direction);
        dLight->setColor(color);
        dLight->autorelease();
        return dLight;
    }
    CC_SAFE_DELETE(dLight);
    return nullptr;
}

/***********    EditorPointLight    *********/
EditorPointLight::EditorPointLight()
{
}

EditorPointLight::~EditorPointLight()
{
}

EditorPointLight * EditorPointLight::create(const cocos2d::Vec3 &position, const cocos2d::Color3B &color, float range)
{
    EditorPointLight * pLight = new (std::nothrow) EditorPointLight();
    if (pLight)
    {
        pLight->setPosition3D(position);
        pLight->setColor(color);
        pLight->_range = range;
        pLight->autorelease();
        return pLight;
    }
    CC_SAFE_DELETE(pLight);
    return nullptr;
}

/***********    EditorSpotLight    *********/
EditorSpotLight::EditorSpotLight()
{
}

EditorSpotLight::~EditorSpotLight()
{
}

EditorSpotLight * EditorSpotLight::create(const cocos2d::Vec3 &direction, const cocos2d::Vec3 &position, const cocos2d::Color3B &color, float innerAngle, float outerAngle, float range)
{
    EditorSpotLight * sLight = new (std::nothrow) EditorSpotLight();
    if (sLight)
    {
        sLight->setRotationFromDirection(direction);
        sLight->setPosition3D(position);
        sLight->setColor(color);
        sLight->setInnerAngle(innerAngle);
        sLight->setOuterAngle(outerAngle);
        sLight->_range = range;
        sLight->autorelease();
        return sLight;
    }
    CC_SAFE_DELETE(sLight);
    return nullptr;
}

/***********    EditorAmbientLight    *********/
EditorAmbientLight::EditorAmbientLight()
{
}

EditorAmbientLight::~EditorAmbientLight()
{
}

EditorAmbientLight* EditorAmbientLight::create(const cocos2d::Color3B &color)
{
    EditorAmbientLight *aLight = new (std::nothrow) EditorAmbientLight();
    if (aLight)
    {
        aLight->setColor(color);
        aLight->autorelease();
        return aLight;
    }
    CC_SAFE_DELETE(aLight);
    return nullptr;
}
