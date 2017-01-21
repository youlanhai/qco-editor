#include "coc_property.h"
#include "runtime/object_factory.h"

#include "property/coc_animation_player_property.h"

NS_COC_BEGIN
void registerCocProperty(ObjectFactory* mgr)
{
    mgr->registerLoader("CocAnimationPlayer", new CocAnimationPlayerProperty());
}
NS_COC_END
