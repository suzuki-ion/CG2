#include "CameraController.h"
#include "Math/Camera.h"

namespace KashipanEngine {

namespace {
inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
inline Vector3 Lerp(const Vector3 &a, const Vector3 &b, float t) { return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t) }; }
}

CameraController::CameraController(Camera *camera) noexcept : camera_(camera) {
    if (camera_) {
        offsetPosition_ = camera_->GetTranslate();
        offsetRotation_ = camera_->GetRotate();
        offsetFov_ = camera_->GetCameraPerspective().fovY;
    }
}

void CameraController::SetCamera(Camera *camera) noexcept {
    camera_ = camera;
    if (camera_) {
        offsetPosition_ = camera_->GetTranslate();
        offsetRotation_ = camera_->GetRotate();
        offsetFov_ = camera_->GetCameraPerspective().fovY;
    }
}

void CameraController::Update() noexcept {
    if (!camera_) { return; }

    constexpr float t = 0.1f; // 補間係数

    // 現在値
    Vector3 curPos = camera_->GetTranslate();
    Vector3 curRot = camera_->GetRotate();
    float curFov = camera_->GetCameraPerspective().fovY;

    // エンド値 = 目標 + オフセット
    Vector3 endPos = targetPosition_ + offsetPosition_;
    Vector3 endRot = targetRotation_ + offsetRotation_;
    float endFov = targetFov_ + offsetFov_;

    // Lerpして適用
    Vector3 nextPos = Lerp(curPos, endPos, t);
    Vector3 nextRot = Lerp(curRot, endRot, t);
    float nextFov = Lerp(curFov, endFov, t);

    camera_->SetTranslate(nextPos);
    camera_->SetRotate(nextRot);

    // FOV変更は CameraPerspective の更新が必要
    auto cp = camera_->GetCameraPerspective();
    cp.fovY = nextFov;
    camera_->SetCameraPerspective(cp);
}

} // namespace KashipanEngine
