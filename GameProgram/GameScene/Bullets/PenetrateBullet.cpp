#include "PenetrateBullet.h"
#include "../BaseEntity.h"
#include <KashipanEngine.h>

void PenetrateBullet::Update() {
    RotationToDirection();
    worldTransform_->translate_ += direction_ * speed_ * Engine::GetDeltaTime();
    worldTransform_->rotate_.z += rotateSpeed_ * Engine::GetDeltaTime();
}

void PenetrateBullet::Draw() {
    bulletModel_->Draw(*worldTransform_);
}

void PenetrateBullet::OnCollision(Collider *other) {
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

