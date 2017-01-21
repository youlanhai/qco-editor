#ifndef PARTICLE_LOADER_H
#define PARTICLE_LOADER_H

#include "node_loader.h"

class PIP_DLL ParticleSystemLoader : public CCNodeLoader
{
public:
    DECLARE_LOADER(CCNodeLoader)

    ParticleSystemLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // PARTICLE_LOADER_H
