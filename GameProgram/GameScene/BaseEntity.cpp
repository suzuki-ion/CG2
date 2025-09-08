#define NOMINMAX

#include <Common/Random.h>
#include <Math/Camera.h>
#include <numbers>

#include "Bullets/NormalBullet.h"
#include "Bullets/HomingBullet.h"
#include "Bullets/PenetrateBullet.h"
#include "CollisionConfig.h"
#include "BaseEntity.h"

using namespace KashipanEngine;

namespace {

Vector3 TransformNormal(const Vector3 &v, const Matrix4x4 &m) {
    // ベクトルを変換するための行列を適用
    Vector3 result;
    result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
    result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
    result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
    return result;
}

} // namespace

BaseEntity::BaseEntity(KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera) {
    model_ = model;
    bulletModel_ = bulletModel;
    camera_ = camera;

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();
    shakeWorldTransform_ = std::make_unique<WorldTransform>();
    cameraWorldTransform_ = std::make_unique<WorldTransform>();
    shakeWorldTransform_->parentTransform_ = cameraWorldTransform_.get();
    worldTransform_->parentTransform_ = shakeWorldTransform_.get();
}

void BaseEntity::SetTargetList(const std::list<BaseEntity *> &targetList) {
    targetList_ = targetList;
    targetList_.sort([this](BaseEntity *a, BaseEntity *b) {
        float distanceA = GetWorldPosition().Distance(a->GetWorldPosition());
        float distanceB = GetWorldPosition().Distance(b->GetWorldPosition());
        return distanceA < distanceB;
    });
}

bool BaseEntity::Shoot() {
    if (shootIntervalTimer_ > 0.0f) {
        return false;
    }

    std::unique_ptr<BaseBullet> bullet;
    switch (shootBulletType_) {
        case BulletType::kNormal:
            bullet = std::make_unique<NormalBullet>(bulletModel_, GetCollisionAttribute(), 0.5f,
                statusParametersCurrent_.attackPower, statusParametersCurrent_.bulletShootSpeed, statusParametersCurrent_.bulletShootInterval);
            break;
        case BulletType::kShotgun:
            break;
        case BulletType::kPenetrate:
            break;
        default:
            break;
    }
    if (!bullet) {
        return false;
    }
    bullet->SetPosition(GetWorldPosition());
    bullet->SetDirection(shootDirection_);
    bullet->SetSpeed(statusParametersCurrent_.bulletShootSpeed);
    shootIntervalTimer_ = bullet->GetInterval();
    addBulletFunction_(std::move(bullet));

    // 発射間隔が1/60を下回らないように制限
    shootIntervalTimer_ = std::max(shootIntervalTimer_, 1.0f / 60.0f);
    return true;
}

void BaseEntity::Draw() {
    // モデルを描画
    model_->Draw(*worldTransform_.get());
}

void BaseEntity::SetDirectionFromVelocity() {
    // 移動方向のベクトルから移動方向を設定
    if (moveDirection_.x < 0.0f) {
        moveDirectionLR_ = MoveDirectionLR::kMoveLeft;
    } else if (moveDirection_.x > 0.0f) {
        moveDirectionLR_ = MoveDirectionLR::kMoveRight;
    } else {
        moveDirectionLR_ = MoveDirectionLR::kMoveNone;
    }
    if (moveDirection_.y < 0.0f) {
        moveDirectionUD_ = MoveDirectionUD::kMoveDown;
    } else if (moveDirection_.y > 0.0f) {
        moveDirectionUD_ = MoveDirectionUD::kMoveUp;
    } else {
        moveDirectionUD_ = MoveDirectionUD::kMoveNone;
    }
}

void BaseEntity::ShakeAnimation() {
    if (shakeAnimationTimer_ <= 0.0f) {
        return;
    }
    float shakeAmount = shakeAnimationTimer_ / shakeAnimationDuration_;
    shakeWorldTransform_->translate_ = {
        GetRandomFloat(-1.0f, 1.0f) * shakeAmount,
        GetRandomFloat(-1.0f, 1.0f) * shakeAmount,
        0.0f
    };
    shakeAnimationTimer_ -= Engine::GetDeltaTime();
}
