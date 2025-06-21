#include "CameraController.h"
#include "Player.h"
#include "Objects/WorldTransform.h"
#include <algorithm>

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const auto& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	targetPosition_.x = targetWorldTransform.translate_.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translate_.y + targetOffset_.y;
	targetPosition_.z = targetWorldTransform.translate_.z + targetOffset_.z;

	// 座標補間でゆったり追従
	camera_->GetTranslatePtr()->x += (targetPosition_.x - camera_->GetTranslatePtr()->x) * kInterpolationRate;
	camera_->GetTranslatePtr()->y += (targetPosition_.y - camera_->GetTranslatePtr()->y) * kInterpolationRate;
	camera_->GetTranslatePtr()->z += (targetPosition_.z - camera_->GetTranslatePtr()->z) * kInterpolationRate;

	// 移動範囲制限
	camera_->GetTranslatePtr()->x = std::clamp(camera_->GetTranslatePtr()->x, movableArea_.left, movableArea_.right);
	camera_->GetTranslatePtr()->y = std::clamp(camera_->GetTranslatePtr()->y, movableArea_.bottom, movableArea_.top);
	camera_->GetTranslatePtr()->z = targetOffset_.z;
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const auto& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->GetTranslatePtr()->x = targetWorldTransform.translate_.x + targetOffset_.x;
	camera_->GetTranslatePtr()->y = targetWorldTransform.translate_.y + targetOffset_.y;
	camera_->GetTranslatePtr()->z = targetWorldTransform.translate_.z + targetOffset_.z;
}
