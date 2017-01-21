#include "csb_animation_loader.h"
#include "../extend/csb_animation.h"

#include "../json_helper.h"

DECLARE_PROPERTY_SETTER(pip::CSBAnimation, file, setAnimationFile, ResourceHolder)

CSBAnimationLoader::CSBAnimationLoader()
{
    REGISTER_PROPERTY_SETTER(file);
}

ObjectType* CSBAnimationLoader::createObject(const JsonHandle &config)
{
    return pip::CSBAnimation::create();
}
