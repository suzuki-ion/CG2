#include "NormalBullet.h"
#include "../BaseEntity.h"
#include <KashipanEngine.h>

void NormalBullet::Update() {
    RotationToDirection();
    worldTransform_->translate_ += direction_ * speed_ * Engine::GetDeltaTime();
}

void NormalBullet::Draw() {
    bulletModel_->Draw(*worldTransform_);
}

void NormalBullet::OnCollision(Collider *other) {
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
