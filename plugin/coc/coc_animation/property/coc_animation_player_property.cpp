#include "coc_animation_player_property.h"
#include "../coc_animation_player.h"
#include "runtime/json_helper.h"

DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, loop, setLoop, bool)
DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, pause, setPause, bool)
DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, remainLastFrame, setRemainLastFrame, bool)
DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, play, play, std::string)
DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, playSpeed, setPlaySpeed, float)
DECLARE_PROPERTY_SETTER(coc::AnimationPlayer, animation, setAnimationFile, ResourceHolder)

DECLARE_PROPERTY_GETTER(coc::AnimationPlayer, pause, isPause, bool)
DECLARE_PROPERTY_GETTER(coc::AnimationPlayer, play, getCurrentClipName, std::string)

CocAnimationPlayerProperty::CocAnimationPlayerProperty()
{
    REGISTER_PROPERTY_SETTER(loop);
    REGISTER_PROPERTY_SETTER(pause);
    REGISTER_PROPERTY_SETTER(remainLastFrame);
    REGISTER_PROPERTY_SETTER(play);
    REGISTER_PROPERTY_SETTER(animation);
    REGISTER_PROPERTY_SETTER(playSpeed);

    REGISTER_PROPERTY_GETTER(pause);
    REGISTER_PROPERTY_GETTER(play);
}

ObjectType* CocAnimationPlayerProperty::createObject(const JsonHandle &config)
{
    return coc::AnimationPlayer::create();
}
