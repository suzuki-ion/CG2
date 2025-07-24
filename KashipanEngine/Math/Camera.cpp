#define NOMINMAX

#include "Camera.h"
#include "RenderingPipeline.h"
#include "Vector2.h"
#include "Base/WinApp.h"
#include "Base/Input.h"
#include "Common/Logs.h"

#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <numbers>

namespace KashipanEngine {

namespace {

// WinAppのポインタ
WinApp *sWinApp_ = nullptr;

Vector3 CalcCameraForward(const Vector3 &rotation) {
    float yaw = rotation.y;
    float pitch = rotation.x;

    return {
        cosf(pitch) * sinf(yaw),
        -sinf(pitch),
        cosf(pitch) * cosf(yaw)
    };
}

Vector3 CalcCameraRight(const Vector3 &rotation) {
    float yaw = rotation.y;

    return {
        sinf(yaw - 3.14f / 2.0f),
        0.0f,
        cosf(yaw - 3.14f / 2.0f)
    };
}

Vector3 CalcCameraUp(const Vector3 &rotation) {
    return CalcCameraForward(rotation).Cross(CalcCameraRight(rotation));
}

} // namespace

void Camera::Initialize(WinApp *winApp) noexcept {
    // nullチェック
    if (!winApp) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    sWinApp_ = winApp;
}

Camera::Camera() {
    coordinateSystem_ = CoordinateSystem::kDecart;
    cameraScale_ = { 1.0f, 1.0f, 1.0f };
    cameraRotate_ = { 0.0f, 0.0f, 0.0f };
    cameraTranslate_ = { 0.0f, 0.0f, 0.0f };
    cameraMatrix_.SetScale(cameraScale_);
    cameraMatrix_.SetRotate(cameraRotate_);
    cameraMatrix_.SetTranslate(cameraTranslate_);
    worldMatrix_.MakeAffine({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
}

Camera::Camera(const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept {
    coordinateSystem_ = CoordinateSystem::kDecart;
    cameraScale_ = cameraScale;
    cameraRotate_ = cameraRotate;
    cameraTranslate_ = cameraTranslate;
    cameraMatrix_.SetScale(cameraScale);
    cameraMatrix_.SetRotate(cameraRotate);
    cameraMatrix_.SetTranslate(cameraTranslate);
    worldMatrix_.MakeAffine({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
}

void Camera::CalculateMatrix() noexcept {
    if (coordinateSystem_ == CoordinateSystem::kDecart) {
        CalculateMatrixForDecart();
    } else if (coordinateSystem_ == CoordinateSystem::kSpherical) {
        CalculateMatrixForSpherical();
    }
}

void Camera::SetCoordinateSystem(CoordinateSystem cameraType) noexcept {
    coordinateSystem_ = cameraType;
    
    if (coordinateSystem_ == CoordinateSystem::kSpherical) {
        sphericalCoordinateSystem_.ToSpherical(cameraTranslate_);
    }
}

void Camera::SetCamera(const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept {
    cameraTranslate_ = cameraTranslate;
    cameraRotate_ = cameraRotate;
    cameraScale_ = cameraScale;
    cameraMatrix_.SetTranslate(cameraTranslate);
    cameraMatrix_.SetRotate(cameraRotate);
    cameraMatrix_.SetScale(cameraScale);
}

void Camera::SetTranslate(const Vector3 &cameraTranslate) noexcept {
    cameraTranslate_ = cameraTranslate;
    cameraMatrix_.SetTranslate(cameraTranslate);
}

void Camera::SetRotate(const Vector3 &cameraRotate) noexcept {
    cameraRotate_ = cameraRotate;
    cameraMatrix_.SetRotate(cameraRotate);
}

void Camera::SetScale(const Vector3 &cameraScale) noexcept {
    cameraScale_ = cameraScale;
    cameraMatrix_.SetScale(cameraScale);
}

void Camera::SetWorldMatrix(const Matrix4x4 &worldMatrix) noexcept {
    worldMatrix_ = worldMatrix;
}

void Camera::SetSphericalCoordinateSystem(const SphericalCoordinateSystem &sphericalCoordinateSystem) noexcept {
    sphericalCoordinateSystem_ = sphericalCoordinateSystem;
}

void Camera::MoveToMouse(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept {
    // ImGuiウィンドウを触ってるときは操作しない
    if (ImGui::IsAnyItemActive()) {
        return;
    }

    if (coordinateSystem_ == CoordinateSystem::kDecart) {
        MoveToMouseForDecart(translateSpeed, rotateSpeed, scaleSpeed);
    } else if (coordinateSystem_ == CoordinateSystem::kSpherical) {
        MoveToMouseForSpherical(translateSpeed, rotateSpeed, scaleSpeed);
    }
}

void Camera::Target(Vector3 *targetPos) noexcept {
    if (!targetPos) {
        return;
    }
    targetPos_ = targetPos;
}

void Camera::CalculateMatrixForDecart() noexcept {
    // ターゲットが設定されている場合はカメラの向きをターゲットに向ける
    if (targetPos_) {
        Vector3 target = *targetPos_;
        Vector3 direction = (cameraTranslate_ - target).Normalize();
        float yaw = atan2f(direction.x, -direction.z);
        float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
        cameraRotate_.x = pitch;
        cameraRotate_.y = -yaw;
    }
    cameraMatrix_.SetTranslate(cameraTranslate_);
    cameraMatrix_.SetRotate(cameraRotate_);
    cameraMatrix_.SetScale(cameraScale_);
    viewMatrix_ = cameraMatrix_.InverseTranslate() * cameraMatrix_.InverseRotate() * cameraMatrix_.InverseScale();
    projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(sWinApp_->GetClientWidth()) / static_cast<float>(sWinApp_->GetClientHeight()), 0.1f, 2048.0f);
    wvpMatrix_ = worldMatrix_ * viewMatrix_ * projectionMatrix_;
    viewportMatrix_ = MakeViewportMatrix(0.0f, 0.0f, static_cast<float>(sWinApp_->GetClientWidth()), static_cast<float>(sWinApp_->GetClientHeight()), 0.0f, 1.0f);
}

void Camera::CalculateMatrixForSpherical() noexcept {
    cameraTranslate_ = sphericalCoordinateSystem_.ToVector3();
    //viewMatrix_ = MakeViewMatrix(cameraTranslate_, sphericalCoordinateSystem_.origin, Vector3(0.0f, 1.0f, 0.0f));
    cameraMatrix_.SetTranslate(cameraTranslate_);
    cameraMatrix_.SetRotate(cameraRotate_);
    cameraMatrix_.SetScale(cameraScale_);
    viewMatrix_ = cameraMatrix_.InverseTranslate() * cameraMatrix_.InverseRotate() * cameraMatrix_.InverseScale();
    projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(sWinApp_->GetClientWidth()) / static_cast<float>(sWinApp_->GetClientHeight()), 0.1f, 2048.0f);
    wvpMatrix_ = worldMatrix_ * viewMatrix_ * projectionMatrix_;
    viewportMatrix_ = MakeViewportMatrix(0.0f, 0.0f, static_cast<float>(sWinApp_->GetClientWidth()), static_cast<float>(sWinApp_->GetClientHeight()), 0.0f, 1.0f);
}

void Camera::MoveToMouseForDecart(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept {
    static Vector2 mousePosDelta;
    mousePosDelta = {
        static_cast<float>(Input::GetMouseDeltaX()),
        static_cast<float>(Input::GetMouseDeltaY())
    };

    if (Input::IsMouseButtonDown(0)) {
        // カメラの向きをもとに上・右方向ベクトルを生成
        Vector3 up = CalcCameraUp(cameraRotate_);
        Vector3 right = CalcCameraRight(cameraRotate_);

        cameraTranslate_ += up * (mousePosDelta.y * translateSpeed);
        cameraTranslate_ += right * (mousePosDelta.x * translateSpeed);
    }
    // 右クリックで回転
    if (Input::IsMouseButtonDown(1)) {
        cameraRotate_.x += mousePosDelta.y * rotateSpeed;
        cameraRotate_.y += mousePosDelta.x * rotateSpeed;
    }
    // ホイールで前後移動
    if (Input::GetMouseWheel() != 0) {
        float mouseWheel = static_cast<float>(Input::GetMouseWheel());
        Vector3 forward = CalcCameraForward(cameraRotate_);
        cameraTranslate_ += forward * (mouseWheel * scaleSpeed);
    }
}

void Camera::MoveToMouseForSpherical(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept {
    static Vector2 mousePosDelta;
    mousePosDelta = {
        static_cast<float>(Input::GetMouseDeltaX()),
        static_cast<float>(Input::GetMouseDeltaY())
    };

    // Shiftキーを押しているときは移動
    if (Input::IsKeyDown(DIK_LSHIFT) || Input::IsKeyDown(DIK_RSHIFT)) {
        if (Input::IsMouseButtonDown(2)) {
            // 半径によって移動速度を調整
            float adjustedTranslateSpeed = translateSpeed * sphericalCoordinateSystem_.radius * 0.01f;
            // カメラの向きをもとに上・右方向ベクトルを生成
            Vector3 up = CalcCameraUp(cameraRotate_);
            Vector3 right = CalcCameraRight(cameraRotate_);
            sphericalCoordinateSystem_.origin += up * (-mousePosDelta.y * adjustedTranslateSpeed);
            sphericalCoordinateSystem_.origin += right * (mousePosDelta.x * adjustedTranslateSpeed);
        }

    } else if (Input::IsMouseButtonDown(2)) {
        sphericalCoordinateSystem_.theta += -mousePosDelta.x * rotateSpeed;
        sphericalCoordinateSystem_.phi += -mousePosDelta.y * rotateSpeed;
        cameraRotate_.x += mousePosDelta.y * rotateSpeed;
        cameraRotate_.y += mousePosDelta.x * rotateSpeed;
    }

    // マウスホイールで半径を変更
    if (Input::GetMouseWheel() != 0) {
        float mouseWheel = static_cast<float>(Input::GetMouseWheel());
        // 半径が小さくなっていく度に縮小率を調整
        float adjustedScaleSpeed = scaleSpeed * sphericalCoordinateSystem_.radius * 0.01f;
        sphericalCoordinateSystem_.radius += -mouseWheel * adjustedScaleSpeed;

        // 半径の範囲を制限
        sphericalCoordinateSystem_.radius = std::max(sphericalCoordinateSystem_.radius, 0.1f);
    }
}

} // namespace KashipanEngine