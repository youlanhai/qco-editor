#ifndef COC_ANIMATION_PLAYER_PROPERTY_H
#define COC_ANIMATION_PLAYER_PROPERTY_H

#include "runtime/loaders/node_loader.h"

class CocAnimationPlayerProperty : public CCNodeLoader
{
public:
    DECLARE_LOADER(CocAnimationPlayerProperty)

    CocAnimationPlayerProperty();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif //COC_ANIMATION_PLAYER_PROPERTY_H
