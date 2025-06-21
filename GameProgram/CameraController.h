#pragma once
#include "Math/Camera.h"

// 前方宣言
class Player;

class CameraController {
public:
	static inline const float kInterpolationRate = 0.2f;
	static inline const float kVelocityBias = 16.0f;

	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 更新
	void Update();

	// リセット
	void Reset();

	// カメラのセット
	void SetCamera(KashipanEngine::Camera* camera) { camera_ = camera; }

	// ターゲットのセット
	void SetTarget(Player* target) { target_ = target; }

	// カメラの移動制限のセット
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

private:
	// カメラ
	KashipanEngine::Camera* camera_ = nullptr;
	// ターゲット
	Player* target_ = nullptr;
	// 追従対象とカメラ座標の差
	KashipanEngine::Vector3 targetOffset_ = { 0.0f, 0.0f, -64.0f };
	// カメラ移動制限
	Rect movableArea_ = { 0.0f, 100.0f, 0.0f, 100.0f };
	// カメラの目標座標
	KashipanEngine::Vector3 targetPosition_ = {0.0f, 0.0f, 0.0f};
};
