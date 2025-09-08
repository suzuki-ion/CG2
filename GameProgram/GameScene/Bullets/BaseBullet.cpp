#include "BaseBullet.h"

using namespace KashipanEngine;

bool BaseBullet::CheckCollision(Collider *other) {
    return false;
}

bool BaseBullet::IsExistTarget() {
    if (!target_) {
        return false;
    }
    if (!isExistTargetFunc_) {
        return false;
    }
    if (isExistTargetFunc_(target_)) {
        return true;
    }
    target_ = nullptr;
    return false;
}

void BaseBullet::RotationToDirection() {
    Vector3 direction = -direction_.Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    worldTransform_->rotate_.x = pitch;
    worldTransform_->rotate_.y = -yaw;
}
