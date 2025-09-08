#define NOMINMAX

#include <KashipanEngine.h> 
#include <Math/Camera.h>
#include <numbers>

#include "Bullets/NormalBullet.h"
#include "Bullets/HomingBullet.h"
#include "Bullets/PenetrateBullet.h"
#include "CollisionConfig.h"
#include "CalculateStatusToPlayer.h"
#include "Player.h"

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

Player::Player(KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera) :
    BaseEntity(model, bulletModel, camera) {
    statusParametersDefault_.healthMax = 100.0f;
    statusParametersDefault_.attackPower = 4.0f;
    statusParametersDefault_.bulletShootSpeed = 8.0f;
    statusParametersDefault_.bulletShootInterval = 1.0f;
    shootDirection_ = { 0.0f, 0.0f, 1.0f };
    shootBulletType_ = BulletType::kNormal;
    
    worldTransform_->translate_.y = -16.0f;
    worldTransform_->translate_.z = 16.0f;
    SetCollisionAttribute(kCollisionAttributePlayer);
    SetCollisionMask(std::bitset<8>(kCollisionAttributePlayer).flip());
    SetRadius(1.0f);
}

void Player::AddExp(float exp) {
    statusParametersCurrent_.exp += exp;
    LevelUp();
}

bool Player::Shoot() {
    if (shootIntervalTimer_ > 0.0f) {
        return false;
    }

    std::unique_ptr<BaseBullet> bullet;
    switch (shootBulletType_) {
        case BulletType::kNormal:
            bullet = std::make_unique<NormalBullet>(bulletModel_, GetCollisionAttribute(), 0.5f,
                statusParametersCurrent_.attackPower, statusParametersCurrent_.bulletShootSpeed, statusParametersCurrent_.bulletShootInterval);
            break;
        case BulletType::kHoming:
            return ShootHomingBullet();
            break;
        case BulletType::kPenetrate:
            bullet = std::make_unique<PenetrateBullet>(bulletModel_, GetCollisionAttribute(), 0.5f,
                statusParametersCurrent_.attackPower, statusParametersCurrent_.bulletShootSpeed, statusParametersCurrent_.bulletShootInterval);
            break;
        default:
            bullet = std::make_unique<NormalBullet>(bulletModel_, GetCollisionAttribute(), 0.5f,
                statusParametersCurrent_.attackPower, statusParametersCurrent_.bulletShootSpeed, statusParametersCurrent_.bulletShootInterval);
            break;
    }
    bullet->SetGiveExpFunc([this](float exp) { this->AddExp(exp); });
    bullet->SetModel(bulletModel_);
    bullet->SetPosition(GetWorldPosition());
    bullet->SetDirection(Vector3(0.0f, 0.0f, 1.0f));
    bullet->SetSpeed(statusParametersCurrent_.bulletShootSpeed);
    shootIntervalTimer_ = bullet->GetInterval();
    shootFunction_(std::move(bullet));

    // 発射間隔が1/60を下回らないように制限
    shootIntervalTimer_ = std::max(shootIntervalTimer_, 1.0f / 60.0f);
    return true;
}

void Player::SetLevel(int level) {
    statusParametersCurrent_.level = level;
    statusParametersCurrent_ = CalculateStatusToPlayer(statusParametersDefault_, statusParametersCurrent_);
}

void Player::OnCollision(Collider *other) {
    if (!isAlive_) {
        return;
    }

    shakeAnimationTimer_ = shakeAnimationDuration_;

    float damage = 0.0f;
    if (dynamic_cast<BaseBullet *>(other) != nullptr) {
        BaseBullet *bullet = static_cast<BaseBullet *>(other);
        damage = bullet->GetAttack();
    } else if (dynamic_cast<BaseEntity *>(other) != nullptr) {
        BaseEntity *entity = static_cast<BaseEntity *>(other);
        damage = entity->GetStatus().attackPower * 2.0f;
    }
    if (damage > 0.0f) {
        Damage(damage);
        showDamageFunc_(damage, GetWorldPosition());
    }
}

void Player::ChangeBulletPrev() {
    int type = static_cast<int>(shootBulletType_);
    type--;
    if (type <= 0) {
        type = static_cast<int>(BulletType::kCount) - 1;
    }
    shootBulletType_ = static_cast<BulletType>(type);
}

void Player::ChangeBulletNext() {
    int type = static_cast<int>(shootBulletType_);
    type++;
    if (type >= static_cast<int>(BulletType::kCount)) {
        type = 1;
    }
    shootBulletType_ = static_cast<BulletType>(type);
}

void Player::Update() {
    isLevelUp_ = false;
    if (!isAlive_) {
        return;
    }

    shootIntervalTimer_ -= Engine::GetDeltaTime();
    Move();
    Rotate();
    LimitPosition();
    ShakeAnimation();
}

void Player::Draw() {
    if (!isAlive_) {
        return;
    }
    // モデルを描画
    model_->Draw(*worldTransform_.get());
}

bool Player::ShootHomingBullet() {
    if (targetList_.size() == 0) {
        return false;
    }
    size_t targetCount = 0;
    for (const auto target: targetList_) {
        if (targetCount >= maxTargetCount_) {
            break;
        }
        std::unique_ptr<BaseBullet> bullet = std::make_unique<HomingBullet>(bulletModel_, GetCollisionAttribute(), 0.5f,
            statusParametersCurrent_.attackPower, statusParametersCurrent_.bulletShootSpeed, statusParametersCurrent_.bulletShootInterval);
        bullet->SetTarget(target);
        bullet->SetGiveExpFunc([this](float exp) { this->AddExp(exp); });
        bullet->SetModel(bulletModel_);
        bullet->SetPosition(GetWorldPosition());
        bullet->SetDirection(Vector3(0.0f, 0.0f, 1.0f));
        bullet->SetSpeed(statusParametersCurrent_.bulletShootSpeed);
        shootIntervalTimer_ = bullet->GetInterval();
        shootFunction_(std::move(bullet));
        targetCount++;
    }
    // 発射間隔が1/60を下回らないように制限
    shootIntervalTimer_ = std::max(shootIntervalTimer_, 1.0f / 60.0f);
    return true;
}

void Player::LevelUp() {
    while (statusParametersCurrent_.exp >= statusParametersCurrent_.expToNextLevel) {
        isLevelUp_ = true;
        maxTargetCount_++;
        statusParametersCurrent_.exp -= statusParametersCurrent_.expToNextLevel;
        SetLevel(statusParametersCurrent_.level + 1);
        // 最大の体力の半分だけ回復
        statusParametersCurrent_.healthCurrent += statusParametersCurrent_.healthMax / 2.0f;
        if (statusParametersCurrent_.healthCurrent > statusParametersCurrent_.healthMax) {
            statusParametersCurrent_.healthCurrent = statusParametersCurrent_.healthMax;
        }
    }
}

void Player::Move() {
    const float kMoveSpeed = moveSpeedSecond * Engine::GetDeltaTime();
    velocity_ += (moveDirection_ - velocity_) * Engine::GetDeltaTime() * 6.0f;
    worldTransform_->translate_ += velocity_ * kMoveSpeed;
    SetDirectionFromVelocity();

    // カメラのワールド変換データを更新
    cameraWorldTransform_->translate_ = camera_->GetTranslate();
    cameraWorldTransform_->rotate_ = camera_->GetRotate();
    cameraWorldTransform_->scale_ = camera_->GetScale();
    cameraWorldTransform_->TransferMatrix();

    // ワールド変換データを更新
    shakeWorldTransform_->TransferMatrix();
    worldTransform_->TransferMatrix();
}

void Player::Rotate() {
    // 調整回転角[ラジアン]
    const float kAdjustmentRotate = 10.0f * std::numbers::pi_v<float> / 180.0f;
    // 1フレームあたりの回転速度
    const float kRotateSpeed = rotateSpeedSecond * Engine::GetDeltaTime();

    // 移動方向に応じて回転を調整
    if (moveDirectionLR_ == MoveDirectionLR::kMoveLeft) {
        rotateAnimationVector_.y = -kAdjustmentRotate;
        rotateAnimationVector_.z = +kAdjustmentRotate;
    } else if (moveDirectionLR_ == MoveDirectionLR::kMoveRight) {
        rotateAnimationVector_.y = +kAdjustmentRotate;
        rotateAnimationVector_.z = -kAdjustmentRotate;
    } else {
        rotateAnimationVector_.y = 0.0f;
        rotateAnimationVector_.z = 0.0f;
    }

    if (moveDirectionUD_ == MoveDirectionUD::kMoveUp) {
        rotateAnimationVector_.x = -kAdjustmentRotate;
    } else if (moveDirectionUD_ == MoveDirectionUD::kMoveDown) {
        rotateAnimationVector_.x = +kAdjustmentRotate;
    } else {
        rotateAnimationVector_.x = 0.0f;
    }

    // 回転角に向かってワールド変換データを回転
    worldTransform_->rotate_ += (rotateAnimationVector_ + rotateVector_ - worldTransform_->rotate_) * kRotateSpeed;
}

void Player::LimitPosition() {
    const Vector3 cameraTranslate(
        cameraWorldTransform_->worldMatrix_.m[3][0],
        cameraWorldTransform_->worldMatrix_.m[3][1],
        cameraWorldTransform_->worldMatrix_.m[3][2]
    );
    CameraPerspective cameraPerspective = camera_->GetCameraPerspective();
    const float kMoveLimitY = 2.0f * worldTransform_->translate_.z * std::tan(cameraPerspective.fovY * 0.5f) * 0.4f;
    const float kMoveLimitX = kMoveLimitY * cameraPerspective.aspectRatio;
    
    // 移動制限範囲を超えたら制限範囲内に戻して速度を0にする
    if (std::clamp(worldTransform_->translate_.x, -kMoveLimitX, kMoveLimitX) != worldTransform_->translate_.x) {
        worldTransform_->translate_.x = std::clamp(worldTransform_->translate_.x, -kMoveLimitX, kMoveLimitX);
        velocity_.x = 0.0f;
    }
    if (std::clamp(worldTransform_->translate_.y, -kMoveLimitY, kMoveLimitY) != worldTransform_->translate_.y) {
        worldTransform_->translate_.y = std::clamp(worldTransform_->translate_.y, -kMoveLimitY, kMoveLimitY);
        velocity_.y = 0.0f;
    }
}