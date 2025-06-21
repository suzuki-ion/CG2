#define NOMINMAX

#include "Enemy.h"
#include "MapChipField.h"
#include "Easings.h"
#include "Base/Input.h"
#include <cassert>
#include <numbers>
#include <algorithm>
#include <array>

using namespace KashipanEngine;

extern const float kBlockWidth;
extern const float kBlockHeight;

namespace {

/// @brief 指定した角の座標計算
/// @param center 中心座標
/// @param corner 角の種類
/// @return 角の座標
KashipanEngine::Vector3 CornerPosition(const Vector3 &center, Enemy::Corner corner) {
    static KashipanEngine::Vector3 offsetTable[Enemy::kNumCorner] = {
        {Enemy::kWidth / 2.0f,  -Enemy::kHeight / 2.0f, 0.0f}, // 右下
        {-Enemy::kWidth / 2.0f, -Enemy::kHeight / 2.0f, 0.0f}, // 左下
        {Enemy::kWidth / 2.0f,  Enemy::kHeight / 2.0f,  0.0f}, // 右上
        {-Enemy::kWidth / 2.0f, Enemy::kHeight / 2.0f,  0.0f}  // 左上
    };

    return KashipanEngine::Vector3(center.x + offsetTable[corner].x, center.y + offsetTable[corner].y, center.z + offsetTable[corner].z);
}

} // namespace

void Enemy::Initialize(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position) {
    // NULLポインタチェック
    assert(model);
    // モデルの設定
    model_ = model;
    // ワールド変換の初期化
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    worldTransform_->translate_ = position;
    worldTransform_->TransferMatrix();
    // 初期で横向きにする
    worldTransform_->rotate_.y = std::numbers::pi_v<float> / 2.0f;
}

void Enemy::Update() {
    // 移動処理
    Move();
    // 当たり判定処理
    CollisionCheck();
    // 旋回処理
    Turn();
    // 歩行アニメーション
    Animation();

    // 行列の転送
    worldTransform_->TransferMatrix();
}

void Enemy::Draw() {
    // 3Dモデルを描画
    model_->Draw(*worldTransform_);
}

void Enemy::OnCollision(const Player *player) {
    static_cast<void>(player);
}

const KashipanEngine::Vector3 Enemy::GetPosition() {
    // ワールド座標を入れる変数
    static KashipanEngine::Vector3 worldPos;
    // ワールド行列の平行移動成分を取得（ワールド座標）
    worldPos.x = worldTransform_->worldMatrix_.m[3][0];
    worldPos.y = worldTransform_->worldMatrix_.m[3][1];
    worldPos.z = worldTransform_->worldMatrix_.m[3][2];
    return worldPos;
}

KashipanEngine::Math::AABB Enemy::GetAABB() {
    // ワールド座標を取得
    KashipanEngine::Vector3 worldPos = GetPosition();

    KashipanEngine::Math::AABB aabb{};

    aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
    aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

    return aabb;
}

void Enemy::Move() {
    // 移動速度
    static const float kWalkSpeed = 0.1f;

    // 重力加速度(下方向)
    static const float kGravityAcceleration = 0.1f;
    // 最大落下速度(下方向)
    static const float kLimitFallSpeed = 2.0f;

    velocity_.x = -kWalkSpeed;

    // 落下速度
    velocity_.y -= kGravityAcceleration;
    // 最大落下速度制限
    velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
}

void Enemy::Animation() {
    // 最初の角度[度]
    static const float kWalkMotionAngleStart = 45.0f;
    // 最後の角度[度]
    static const float kWalkMotionAngleEnd = -45.0f;
    // アニメーションの周期となる時間[秒]
    static const float kWalkMotionTime = 1.0f;

    // タイマーを加算
    walkTimer_ += 0.1f;
    // 回転アニメーション
    float param = std::sin((std::numbers::pi_v<float> *2.0f) * (walkTimer_ / kWalkMotionTime));
    float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
    // 回転角度をラジアンに変換
    worldTransform_->rotate_.x = std::numbers::pi_v<float> *degree / 180.0f;
    // タイマーが周期を超えたらリセット
    if (walkTimer_ >= kWalkMotionTime) {
        walkTimer_ = 0.0f;
    }
}

void Enemy::CollisionCheck() {
    CollisionMapInfo collisionMapInfo;
    collisionMapInfo.velocity = velocity_;

    CollisionCheckUp(collisionMapInfo);
    CollisionCheckDown(collisionMapInfo);
    CollisionCheckLeft(collisionMapInfo);
    CollisionCheckRight(collisionMapInfo);
    ApplyCollisionResult(collisionMapInfo);
    SwitchOnGroundState(collisionMapInfo);
    // 天井に接している場合の処理
    OnHitCeiling(collisionMapInfo);
    // 壁に接している場合の処理
    OnHitWall(collisionMapInfo);
}

void Enemy::CollisionCheckUp(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.y <= 0.0f) {
        // 上方向に移動していない場合は当たり判定しない
        return;
    }

    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x,
                worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };

    bool isHit = false;
    // 上辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftTop]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightTop]));
    // 上辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftTop]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.y = std::max(0.0f, (rect.bottom - worldTransform_->translate_.y) - (Enemy::kHeight / 2.0f));
    // 天井に当たったことを記録する
    collisionMapInfo.isHitUp = true;
}

void Enemy::CollisionCheckDown(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.y >= 0.0f) {
        // 下方向に移動していない場合は当たり判定しない
        return;
    }

    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };

    bool isHit = false;
    // 下辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]));
    // 下辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.y = std::min(0.0f, (rect.top - worldTransform_->translate_.y) + (Enemy::kHeight / 2.0f));
    collisionMapInfo.isHitGround = true;
}

void Enemy::CollisionCheckLeft(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.x >= 0.0f) {
        // 左方向に移動していない場合は当たり判定しない
        return;
    }

    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };

    bool isHit = false;
    // 左辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftTop]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]));
    // 左辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.x = std::max(0.0f, (rect.right - worldTransform_->translate_.x) - (Enemy::kWidth / 2.0f));
    collisionMapInfo.isHitWall = true;
}

void Enemy::CollisionCheckRight(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.x <= 0.0f) {
        // 右方向に移動していない場合は当たり判定しない
        return;
    }
    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };
    bool isHit = false;
    // 右辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightTop]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]));
    // 右辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.x = std::min(0.0f, (rect.left - worldTransform_->translate_.x) + (Enemy::kWidth / 2.0f));
    collisionMapInfo.isHitWall = true;
}

void Enemy::ApplyCollisionResult(const CollisionMapInfo &collisionMapInfo) {
    // 速度を適用する
    worldTransform_->translate_.x += collisionMapInfo.velocity.x;
    worldTransform_->translate_.y += collisionMapInfo.velocity.y;
    worldTransform_->translate_.z += collisionMapInfo.velocity.z;
    // 速度を反映する
    velocity_.x = collisionMapInfo.velocity.x;
    velocity_.y = collisionMapInfo.velocity.y;
    velocity_.z = collisionMapInfo.velocity.z;
}

void Enemy::OnHitCeiling(const CollisionMapInfo &collisionMapInfo) {
    // 天井に当たった場合は上方向の速度を0にする
    if (collisionMapInfo.isHitUp) {
        velocity_.y = 0.0f;
    }
}

void Enemy::OnHitWall(const CollisionMapInfo &collisionMapInfo) {
    const float kAttenuationWall = 0.1f;

    // 壁接触による減速
    if (collisionMapInfo.isHitWall) {
        velocity_.x *= (1.0f - kAttenuationWall);
    }
}

void Enemy::SwitchOnGroundState(CollisionMapInfo &collisionMapInfo) {
    static const float kAttenuationLanding = 0.1f;
    static const float kDownPosPlus = 2.0f;

    if (onGround_) {
        // 接地状態の処理
        if (velocity_.y > 0.0f) {
            onGround_ = false;
        } else {
            if (!collisionMapInfo.isHitGround) {
                // 落下開始
                onGround_ = false;
            }
        }

    } else {
        // 空中状態の処理
        if (collisionMapInfo.isHitGround) {
            // 着地状態に切り替える
            onGround_ = true;
            // 着地時にX速度を減衰
            velocity_.x *= (1.0f - kAttenuationLanding);
            // Y速度をリセット
            velocity_.y = 0.0f;
        }
    }
}

void Enemy::Turn() {
    // 左右の自キャラ角度テーブル
    static const float destinationRotationYTable[] = {
        std::numbers::pi_v<float> / 2.0f,
        std::numbers::pi_v<float> *3.0f / 2.0f,
    };
    // 旋回時間<秒>
    const float kTimeTurn = 0.3f;

    if (turnTimer_ <= kTimeTurn) {
        // 旋回タイマーを1/60秒分カウントする
        turnTimer_ += 1.0f / 60.0f;

        // 状態に応じた角度を取得する
        float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
        // 自キャラの角度を設定する
        worldTransform_->rotate_.y = Ease::OutCubic(turnTimer_ / kTimeTurn, turnFirstRotationY_, destinationRotationY);
    }
}
