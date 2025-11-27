#pragma once
#include "Math/Vector3.h"

namespace KashipanEngine {

class Camera;

class CameraController {
public:
    CameraController() noexcept = default;
    explicit CameraController(Camera *camera) noexcept;

    void SetCamera(Camera *camera) noexcept;
    Camera *GetCamera() const noexcept { return camera_; }

    // ターゲット値の設定/取得
    void SetTargetPosition(const Vector3 &pos) noexcept { targetPosition_ = pos; }
    void SetTargetRotation(const Vector3 &rot) noexcept { targetRotation_ = rot; }
    void SetTargetFov(float fovY) noexcept { targetFov_ = fovY; }

    const Vector3 &GetTargetPosition() const noexcept { return targetPosition_; }
    const Vector3 &GetTargetRotation() const noexcept { return targetRotation_; }
    float GetTargetFov() const noexcept { return targetFov_; }

    // オフセットの設定/取得（ターゲット値に加算される）
    void SetOffsetPosition(const Vector3 &pos) noexcept { offsetPosition_ = pos; }
    void SetOffsetRotation(const Vector3 &rot) noexcept { offsetRotation_ = rot; }
    void SetOffsetFov(float fovY) noexcept { offsetFov_ = fovY; }
    const Vector3 &GetOffsetPosition() const noexcept { return offsetPosition_; }
    const Vector3 &GetOffsetRotation() const noexcept { return offsetRotation_; }

    // 毎フレーム、現在値->(ターゲット+オフセット)へ線形補間で寄せる
    void Update() noexcept;

private:
    Camera *camera_ = nullptr;

    // 目標値（end値）
    Vector3 targetPosition_{0.0f, 0.0f, 0.0f};
    Vector3 targetRotation_{0.0f, 0.0f, 0.0f};
    float targetFov_ = 0.0f;

    // オフセット
    Vector3 offsetPosition_{0.0f, 0.0f, 0.0f};
    Vector3 offsetRotation_{0.0f, 0.0f, 0.0f};
    float offsetFov_ = 0.0f;
};

} // namespace KashipanEngine
