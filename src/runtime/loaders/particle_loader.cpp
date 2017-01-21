#include "particle_loader.h"
#include "../json_helper.h"
#include <2d/CCParticleSystemQuad.h>

DEFINE_PROPERTY_SETTER(cocos2d::ParticleSystemQuad, positionType)
{
    int type;
    config >> type;
    node->setPositionType(cocos2d::ParticleSystem::PositionType(type));
}

#ifdef BUILD_EDITOR
DEFINE_PROPERTY_SETTER(cocos2d::ParticleSystemQuad, plist)
{
    std::string path;
    config >> path;
    RESOLVE_FILE_RESOURCE(path);

    node->initWithFile(path);
}
#endif

ParticleSystemLoader::ParticleSystemLoader()
{
    REGISTER_PROPERTY_SETTER(positionType);
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(plist);
#endif
}

ObjectType* ParticleSystemLoader::createObject(const JsonHandle &config)
{
    std::string path;
    config["plist"] >> path;
    if(!path.empty())
    {
        RESOLVE_FILE_RESOURCE(path);
        return cocos2d::ParticleSystemQuad::create(path);
    }
    else
    {
        return cocos2d::ParticleSystemQuad::create();
    }
}
