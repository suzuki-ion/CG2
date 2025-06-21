#define NOMINMAX

#include "Player.h"
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
KashipanEngine::Vector3 CornerPosition(const Vector3& center, Player::Corner corner) {
	static KashipanEngine::Vector3 offsetTable[Player::kNumCorner] = {
	    {Player::kWidth / 2.0f,  -Player::kHeight / 2.0f, 0.0f}, // 右下
	    {-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0.0f}, // 左下
	    {Player::kWidth / 2.0f,  Player::kHeight / 2.0f,  0.0f}, // 右上
	    {-Player::kWidth / 2.0f, Player::kHeight / 2.0f,  0.0f}  // 左上
	};

	return KashipanEngine::Vector3(
		center.x + offsetTable[corner].x,
		center.y + offsetTable[corner].y,
		center.z + offsetTable[corner].z
	);
}

} // namespace

void Player::Initialize(KashipanEngine::Model *model, KashipanEngine::Model *modelAttack, const KashipanEngine::Vector3 &position) {
	// NULLポインタチェック
	assert(model);
    assert(modelAttack);
	// モデルの設定
	model_ = model;
    modelAttack_ = modelAttack;
    // 攻撃用モデルはデフォルトで非表示にする
    for (auto &models : modelAttack_->GetModels()) {
        // アルファ値を0にして非表示にする
        models.GetStatePtr().material->color.w = 0.0f;
    }
	// ワールド変換の初期化
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
	worldTransform_->translate_ = position;
    worldTransform_->TransferMatrix();
	// 初期で横向きにする
	worldTransform_->rotate_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// 振る舞いの切り替え
	SwitchBehaivior();

	// 振る舞いの更新
	switch (behaivior_) {
	case Behaivior::kRoot:
		BehaiviorRootUpdate();
		break;
	case Behaivior::kAttack:
		BehaiviorAttackUpdate();
		break;
	}

	// 当たり判定処理
	CollisionCheck();
	// 旋回処理
	Turn();

	// 行列の転送
	worldTransform_->TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(*worldTransform_);
	modelAttack_->Draw();
}

void Player::OnCollision(const Enemy* enemy) {
	static_cast<void>(enemy);
	// デスフラグを立てる
	isDead_ = true;
}

const KashipanEngine::Vector3 Player::GetPosition() {
	return Vector3(
		worldTransform_->worldMatrix_.m[3][0],
		worldTransform_->worldMatrix_.m[3][1],
		worldTransform_->worldMatrix_.m[3][2]
	);
}

KashipanEngine::Math::AABB Player::GetAABB() {
	// ワールド座標を取得
	KashipanEngine::Vector3 worldPos = GetPosition();

	KashipanEngine::Math::AABB aabb{};

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::BehaiviorRootUpdate() {
	// 移動処理
	Move();
	// shiftキーが押されたら振る舞いを攻撃に切り替える
	if (Input::IsKeyDown(DIK_LSHIFT) ||
		Input::IsKeyDown(DIK_RSHIFT)) {
		behaiviorRequest_ = Behaivior::kAttack;
	}
}

void Player::Move() {
	// 加速度
	static const float kAcceleration = 0.1f;
	// 減衰度
	static const float kAttenuation = 0.1f;
	// 最大速度
	static const float kLimitRunSpeed = 2.0f;

	// 重力加速度(下方向)
	static const float kGravityAcceleration = 0.1f;
	// 最大落下速度(下方向)
	static const float kLimitFallSpeed = 2.0f;
	// ジャンプ初速(上方向)
	static const float kJumpAcceleration = 2.0f;

	if (onGround_) {
		// 左右移動操作
		if (Input::IsKeyDown(DIK_RIGHT) || Input::IsKeyDown(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::IsKeyDown(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_->rotate_.y;
					turnTimer_ = 0.0f;
				}
			}
			if (Input::IsKeyDown(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_->rotate_.y;
					turnTimer_ = 0.0f;
				}
			}
			// 加減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ操作
		if (Input::IsKeyDown(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}

	}

	// 落下速度
	velocity_.y -= kGravityAcceleration;
	// 最大落下速度制限
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
}

void Player::CollisionCheck() {
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity = velocity_;

	// 上下処理
	CollisionCheckUp(collisionMapInfo);
	CollisionCheckDown(collisionMapInfo);
	worldTransform_->translate_.y += collisionMapInfo.velocity.y;
	velocity_.y = collisionMapInfo.velocity.y;
	collisionMapInfo.velocity.y = 0.0f;

	// 左右処理
	CollisionCheckLeft(collisionMapInfo);
	CollisionCheckRight(collisionMapInfo);
	worldTransform_->translate_.x += collisionMapInfo.velocity.x;
	velocity_.x = collisionMapInfo.velocity.x;
	collisionMapInfo.velocity.x = 0.0f;

	SwitchOnGroundState(collisionMapInfo);
	// 天井に接している場合の処理
	OnHitCeiling(collisionMapInfo);
	// 壁に接している場合の処理
	OnHitWall(collisionMapInfo);
}

void Player::CollisionCheckUp(CollisionMapInfo& collisionMapInfo) {
	if (collisionMapInfo.velocity.y <= 0.0f) {
		// 上方向に移動していない場合は当たり判定しない
		return;
	}

	// 移動後の4つの角の座標
	std::array<KashipanEngine::Vector3, Player::kNumCorner> positionNew{};
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(
		    KashipanEngine::Vector3(
				worldTransform_->translate_.x + collisionMapInfo.velocity.x,
				worldTransform_->translate_.y + collisionMapInfo.velocity.y,
		        worldTransform_->translate_.z + collisionMapInfo.velocity.z),
			static_cast<Player::Corner>(i));
	};

	bool isHit = false;
	// 上辺のインデックス全てを取得
	std::vector<IndexSet> indexSets;
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kLeftTop]));
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kRightTop]));
	// 上辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
	for (const auto& indexSet : indexSets) {
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
	IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Player::kLeftTop]);
	// めり込み先ブロックの範囲矩形
	Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
	collisionMapInfo.velocity.y = std::max(0.0f, (rect.bottom - worldTransform_->translate_.y) - (Player::kHeight / 2.0f));
	// 天井に当たったことを記録する
	collisionMapInfo.isHitUp = true;
}

void Player::CollisionCheckDown(CollisionMapInfo& collisionMapInfo) {
	if (collisionMapInfo.velocity.y >= 0.0f) {
		// 下方向に移動していない場合は当たり判定しない
		return;
	}

	// 移動後の4つの角の座標
	std::array<KashipanEngine::Vector3, Player::kNumCorner> positionNew{};
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(
		    KashipanEngine::Vector3(
		        worldTransform_->translate_.x + collisionMapInfo.velocity.x,
				worldTransform_->translate_.y + collisionMapInfo.velocity.y,
		        worldTransform_->translate_.z + collisionMapInfo.velocity.z),
		    static_cast<Player::Corner>(i));
	};

	bool isHit = false;
	// 下辺のインデックス全てを取得
	std::vector<IndexSet> indexSets;
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kLeftBottom]));
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kRightBottom]));
	// 下辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
	for (const auto& indexSet : indexSets) {
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
	IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Player::kLeftBottom]);
	// めり込み先ブロックの範囲矩形
	Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
	collisionMapInfo.velocity.y = std::min(0.0f, (rect.top - worldTransform_->translate_.y) + (Player::kHeight / 2.0f));
	collisionMapInfo.isHitGround = true;
}

void Player::CollisionCheckLeft(CollisionMapInfo& collisionMapInfo) {
	if (collisionMapInfo.velocity.x >= 0.0f) {
		// 左方向に移動していない場合は当たり判定しない
		return;
	}

	// 移動後の4つの角の座標
	std::array<KashipanEngine::Vector3, Player::kNumCorner> positionNew{};
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(
		    KashipanEngine::Vector3(
		        worldTransform_->translate_.x + collisionMapInfo.velocity.x,
				worldTransform_->translate_.y + collisionMapInfo.velocity.y,
		        worldTransform_->translate_.z + collisionMapInfo.velocity.z),
		    static_cast<Player::Corner>(i));
	};

	bool isHit = false;
	// 左辺のインデックス全てを取得
	std::vector<IndexSet> indexSets;
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kLeftTop]));
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kLeftBottom]));
	// 左辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
	for (const auto& indexSet : indexSets) {
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
	IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Player::kLeftTop]);
	// めり込み先ブロックの範囲矩形
	Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
	collisionMapInfo.velocity.x = std::max(0.0f, (rect.right - worldTransform_->translate_.x) - (Player::kWidth / 2.0f));
	collisionMapInfo.isHitWall = true;
}

void Player::CollisionCheckRight(CollisionMapInfo &collisionMapInfo) {
	if (collisionMapInfo.velocity.x <= 0.0f) {
		// 右方向に移動していない場合は当たり判定しない
		return;
	}
	// 移動後の4つの角の座標
	std::array<KashipanEngine::Vector3, Player::kNumCorner> positionNew{};
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(
		    KashipanEngine::Vector3(
		        worldTransform_->translate_.x + collisionMapInfo.velocity.x,
				worldTransform_->translate_.y + collisionMapInfo.velocity.y,
		        worldTransform_->translate_.z + collisionMapInfo.velocity.z),
		    static_cast<Player::Corner>(i));
	};
	bool isHit = false;
	// 右辺のインデックス全てを取得
	std::vector<IndexSet> indexSets;
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kRightTop]));
	indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Player::kRightBottom]));
	// 右辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
	for (const auto& indexSet : indexSets) {
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
	IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Player::kRightTop]);
	// めり込み先ブロックの範囲矩形
	Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
	collisionMapInfo.velocity.x = std::min(0.0f, (rect.left - worldTransform_->translate_.x) + (Player::kWidth / 2.0f));
	collisionMapInfo.isHitWall = true;
}

void Player::ApplyCollisionResult(const CollisionMapInfo& collisionMapInfo) {
	// 速度を適用する
	worldTransform_->translate_.x += collisionMapInfo.velocity.x;
	worldTransform_->translate_.y += collisionMapInfo.velocity.y;
	worldTransform_->translate_.z += collisionMapInfo.velocity.z;
	// 速度を反映する
	velocity_.x = collisionMapInfo.velocity.x;
	velocity_.y = collisionMapInfo.velocity.y;
	velocity_.z = collisionMapInfo.velocity.z;
}

void Player::OnHitCeiling(const CollisionMapInfo& collisionMapInfo) {
	// 天井に当たった場合は上方向の速度を0にする
	if (collisionMapInfo.isHitUp) {
		velocity_.y = 0.0f;
	}
}

void Player::OnHitWall(const CollisionMapInfo& collisionMapInfo) {
	const float kAttenuationWall = 0.1f;

	// 壁接触による減速
	if (collisionMapInfo.isHitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::SwitchOnGroundState(CollisionMapInfo& collisionMapInfo) {
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

void Player::Turn() {
	// 左右の自キャラ角度テーブル
	static const float destinationRotationYTable[] = {
	    std::numbers::pi_v<float> / 2.0f,
	    std::numbers::pi_v<float> * 3.0f / 2.0f,
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

void Player::BehaiviorAttackUpdate() {
	// 溜めの時間
	static const int kAttackDuration = 2;
	// 突進時間
	static const int kRushDuration = 4;
	// 余韻の時間
	static const int kAfterDuration = 4;

	// 予備動作
	attackParameter_++;

	float t;
	switch (attackPhase_) {
	case AttackPhase::kStore:
		t = static_cast<float>(attackParameter_) / kAttackDuration;
		worldTransform_->scale_.z = Ease::OutCubic(t, 1.0f, 0.3f);
		worldTransform_->scale_.y = Ease::OutCubic(t, 1.0f, 1.6f);
		if (attackParameter_ >= kAttackDuration) {
			// 溜めが終わったら突進へ
			attackPhase_ = AttackPhase::kRush;
			attackParameter_ = 0;
            // 攻撃用モデルを表示する
            for (auto &model : modelAttack_->GetModels()) {
                // アルファ値を255にして表示する
                model.GetStatePtr().material->color.w = 255.0f;
                // プレイヤーの向きに合わせてモデルのスケールを設定
                if (lrDirection_ == LRDirection::kRight) {
                    model.GetStatePtr().transform->scale.x = 1.0f;
                } else {
                    model.GetStatePtr().transform->scale.x = -1.0f;
                }
            }
		}
		break;

	case AttackPhase::kRush:
		t = static_cast<float>(attackParameter_) / kRushDuration;
		worldTransform_->scale_.z = Ease::OutCubic(t, 0.3f, 1.3f);
		worldTransform_->scale_.y = Ease::InCubic(t, 1.6f, 0.7f);
		if (attackParameter_ >= kRushDuration) {
			// 突進が終わったら余韻へ
			attackPhase_ = AttackPhase::kAfter;
			attackParameter_ = 0;
		}
		// 向いてる方向に突進
		if (lrDirection_ == LRDirection::kRight) {
			velocity_.x = 2.0f;
		} else {
			velocity_.x = -2.0f;
		}
		break;

	case AttackPhase::kAfter:
		t = static_cast<float>(attackParameter_) / kAfterDuration;
		worldTransform_->scale_.z = Ease::InCubic(t, 1.3f, 1.0f);
		worldTransform_->scale_.y = Ease::InCubic(t, 0.7f, 1.0f);
        // 攻撃用モデルのアルファ値を徐々に0にする
        for (auto &model : modelAttack_->GetModels()) {
            // アルファ値を減衰させる
            model.GetStatePtr().material->color.w = std::max(0.0f, (1.0f - t) * 255.0f);
        }
		if (attackParameter_ >= kAfterDuration) {
			// 余韻が終わったら通常行動へ
			behaiviorRequest_ = Behaivior::kRoot;
			attackPhase_ = AttackPhase::kStore;
			attackParameter_ = 0;
			return;
		}
		break;
	}

	// 攻撃用モデルの位置をプレイヤーの位置に合わせる
	for (auto &model : modelAttack_->GetModels()) {
		model.GetStatePtr().transform->translate = worldTransform_->translate_;
	}
}

void Player::SwitchBehaivior() {
	if (behaiviorRequest_ == Behaivior::kUnknown) {
		return;
	}
	// 振る舞いを変更
	behaivior_ = behaiviorRequest_;
	// 各振る舞いごとの初期化を実行
	switch (behaivior_) {
	case Behaivior::kRoot:
		// 通常行動の初期化
		BehaiviorRootInitialize();
		break;
	case Behaivior::kAttack:
		// 攻撃行動の初期化
		BehaiviorAttackInitialize();
		break;
	}
	behaiviorRequest_ = Behaivior::kUnknown;
}

void Player::BehaiviorRootInitialize() {}

void Player::BehaiviorAttackInitialize() {
	// 攻撃ギミックの経過時間カウンターをリセット
	attackParameter_ = 0;
}
