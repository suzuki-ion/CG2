#include "DeathParticle.h"
#include "Player.h"
#include <cassert>
#include <numbers>
#include <algorithm>

using namespace KashipanEngine;

void DeathParticle::Initialize(KashipanEngine::Model* model) {
	// NULLポインタチェック
	assert(model);
	// モデルの設定
	model_ = model;
}

void DeathParticle::Update() {
	// 存続時間（消滅までの時間）<秒>
	static const float kDuration = 1.0f;
	// 移動の速さ
	static const float kSpeed = 0.1f;
	// 分割した1個分の角度
	static const float kAngleUnit = (2.0f * std::numbers::pi_v<float>) / static_cast<float>(kNumParticle);

	// 終了フラグが立っている場合は何もしない
	if (isFinished_) {
		return;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	int i = 0;
	for (auto& worldTransform : worldTransforms_) {
		// 基本となる速度ベクトル
		Vector3 velocity = { kSpeed, 0.0, 0.0f };
		// 回転角を計算
		float angle = kAngleUnit * static_cast<float>(i);
		// Z軸まわり回転行列
		Matrix4x4 matrixRotateZ{};
		matrixRotateZ.MakeRotateZ(angle);
		velocity = velocity.Transform(matrixRotateZ);
		// 移動処理
		worldTransform.translate_.x += velocity.x;
		worldTransform.translate_.y += velocity.y;
		worldTransform.translate_.z += velocity.z;
		// 色の設定
		for (auto &model : model_->GetModels()) {
			model.GetStatePtr().material->color.w = std::clamp((1.0f - (counter_ / kDuration)) * 255.0f, 0.0f, 255.0f);
		}

		// iをインクリメント
		++i;
	}

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		isFinished_ = true;
	}
}

void DeathParticle::Draw() {
	// 終了フラグが立っている場合は何もしない
	if (isFinished_) {
		return;
	}

	// 3Dモデルを描画
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform);
	}
}

void DeathParticle::Emit(const KashipanEngine::Vector3& position) {
	// パーティクルの位置を設定
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.translate_ = position;
	}
	// 終了フラグをリセット
	isFinished_ = false;
	// カウンターをリセット
	counter_ = 0.0f;
}
