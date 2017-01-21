#include "physicsbody_loader.h"
#include "../json_helper.h"
#include <physics/CCPhysicsBody.h>

DECLARE_PROPERTY_SETTER(cocos2d::PhysicsBody, tag, setTag, int)
DECLARE_PROPERTY_SETTER(cocos2d::PhysicsBody, dynamic, setDynamic, bool)
DECLARE_PROPERTY_SETTER(cocos2d::PhysicsBody, mass, setMass, float)

PhysicsBodyLoader::PhysicsBodyLoader()
{
    REGISTER_PROPERTY_SETTER(tag);
    REGISTER_PROPERTY_SETTER(dynamic);
    REGISTER_PROPERTY_SETTER(mass);
}

ObjectType *PhysicsBodyLoader::createObject(const JsonHandle &/*config*/)
{
    return cocos2d::PhysicsBody::create();
}
