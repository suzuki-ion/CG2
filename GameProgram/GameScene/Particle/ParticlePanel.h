#pragma once
#include "BaseParticle.h"

class ParticlePanel : public BaseParticle {
public:
    ParticlePanel(
        KashipanEngine::Model *model,
        const KashipanEngine::Vector3 &position,
        const KashipanEngine::Vector3 &direction,
        const KashipanEngine::Vector3 &rotation,
        float speed,
        float rotationSpeed,
        float lifeTime = 0.0f,
        bool isEraseByLife = false
    ) : BaseParticle(model, position, direction, rotation, speed, rotationSpeed, lifeTime, isEraseByLife) {}
};