#include "HomingBullet.h"
#include "../BaseEntity.h"
#include <KashipanEngine.h>

void HomingBullet::Update() {
    if (lifeTime_ <= 0.0f) {
        isAlive_ = false;
        return;
    }
    if (IsExistTarget()) {
        lifeTime_ -= Engine::GetDeltaTime();
        direction_ = (target_->GetWorldPosition() - GetWorldPosition()).Normalize();
    }
    RotationToDirection();
    worldTransform_->translate_ += direction_ * speed_ * Engine::GetDeltaTime();
}

void HomingBullet::Draw() {
    bulletModel_->Draw(*worldTransform_);
}

void HomingBullet::OnCollision(Collider *other) {
    isAlive_ = false;
    if (dynamic_cast<BaseEntity *>(other) == nullptr) {
        return;
    }

    BaseEntity *entity = dynamic_cast<BaseEntity *>(other);
    entity->Damage(attack_);
    showDamageFunc_(attack_, entity->GetWorldPosition());
    if (entity->IsAlive()) {
        return;
    }

    if (!giveExpFunc_) {
        return;
    }
    StatusParameters params = dynamic_cast<BaseEntity *>(other)->GetStatus();
    giveExpFunc_(params.exp);
}
