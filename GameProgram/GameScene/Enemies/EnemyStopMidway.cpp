#define NOMINMAX

#include <KashipanEngine.h> 
#include <Math/Camera.h>
#include <numbers>

#include "../CollisionConfig.h"
#include "../CalculateStatusToEnemy.h"
#include "EnemyStopMidway.h"

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

EnemyStopMidway::EnemyStopMidway(const KashipanEngine::Vector3 &pos, KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera) :
    BaseEntity(model, bulletModel, camera) {
    statusParametersDefault_.healthMax = 4.0f;
    statusParametersDefault_.attackPower = 1.0f;
    statusParametersDefault_.bulletShootSpeed = 8.0f;
    statusParametersDefault_.bulletShootInterval = 3.0f;
    shootDirection_ = { 0.0f, 0.0f, -1.0f };

    worldTransform_->translate_ = pos;
    worldTransform_->rotate_.y = std::numbers::pi_v<float>;
    rotateVector_.y = std::numbers::pi_v<float>;
    SetCollisionAttribute(kCollisionAttributeEnemy);
    SetCollisionMask(std::bitset<8>(kCollisionAttributeEnemy).flip());
    SetRadius(1.0f);

    moveDirection_ = Vector3(0.0f, 0.0f, -1.0f);
    moveSpeedSecond = 16.0f;
}

void EnemyStopMidway::SetLevel(int level) {
    statusParametersCurrent_.level = level;
    statusParametersCurrent_ = CalculateStatusToEnemy(statusParametersDefault_, statusParametersCurrent_);
    statusParametersCurrent_.healthCurrent = statusParametersCurrent_.healthMax;
}

void EnemyStopMidway::OnCollision(Collider *other) {
    static_cast<void>(other);
    shakeAnimationTimer_ = shakeAnimationDuration_;
}

void EnemyStopMidway::Update() {
    shootIntervalTimer_ -= Engine::GetDeltaTime();
    Move();
    Rotate();
    if (worldTransform_->translate_.z <= 32.0f) {
        Shoot();
    }
    ShakeAnimation();
}

void EnemyStopMidway::Draw() {
    // モデルを描画
    model_->Draw(*worldTransform_.get());
}

void EnemyStopMidway::Move() {
    // Z座標が32.0f以下になったら停止
    if (worldTransform_->translate_.z <= 32.0f) {
        moveDirection_ = Vector3(0.0f, 0.0f, 0.0f);
    }

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

void EnemyStopMidway::Rotate() {
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