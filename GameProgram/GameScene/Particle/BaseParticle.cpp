#include "BaseParticle.h"
#include <KashipanEngine.h>

using namespace KashipanEngine;

BaseParticle::BaseParticle(Model *model, const Vector3 &position, const Vector3 &direction, const Vector3 &rotation, float speed, float rotationSpeed, float lifeTime, bool isEraseByLife) {
    worldTransform_ = std::make_unique<WorldTransform>();

    model_ = model;
    worldTransform_->translate_ = position;
    direction_ = direction.Normalize();
    rotation_ = rotation.Normalize();
    speed_ = speed;
    rotationSpeed_ = rotationSpeed;
    lifeTime_ = lifeTime;
    isEraseByLife_ = isEraseByLife;
}

void BaseParticle::UpdateCommon() {
    if (isAlive_ == false) {
        return;
    }

    worldTransform_->translate_ += direction_ * speed_ * Engine::GetDeltaTime();
    worldTransform_->rotate_ += rotation_ * rotationSpeed_ * Engine::GetDeltaTime();
    
    if (!isEraseByLife_) {
        return;
    }
    elapsedTime_ += Engine::GetDeltaTime();
    if (elapsedTime_ >= lifeTime_) {
        isAlive_ = false;
    }
}

void BaseParticle::DrawCommon() {
    if (model_ == nullptr) {
        return;
    }
    if (isAlive_ == false) {
        return;
    }
    model_->Draw(*worldTransform_);
}