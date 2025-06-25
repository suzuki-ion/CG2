#pragma once
#include "AffineMatrix.h"
#include "SphericalCoordinateSystem.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;

class Camera {
public:
    enum class CoordinateSystem {
        kDecart,
        kSpherical,
    };

    static void Initialize(WinApp *winApp) noexcept;

    Camera();
    Camera(const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept;

    /// @brief 各行列を計算する
    void CalculateMatrix() noexcept;

    /// @brief カメラの座標系を設定する
    /// @param coordinateSystem 座標系
    void SetCoordinateSystem(CoordinateSystem coordinateSystem) noexcept;

    /// @brief カメラの行列を設定する
    /// @param cameraTranslate 平行移動
    /// @param cameraRotate 回転
    /// @param cameraScale 拡大縮小
    void SetCamera(const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept;

    /// @brief カメラの拡大縮小行列を設定する
    /// @param cameraScale 拡大縮小
    void SetScale(const Vector3 &cameraScale) noexcept;

    /// @brief カメラの回転行列を設定する
    /// @param cameraRotate 回転
    void SetRotate(const Vector3 &cameraRotate) noexcept;

    /// @brief カメラの平行移動行列を設定する
    /// @param cameraTranslate 平行移動
    void SetTranslate(const Vector3 &cameraTranslate) noexcept;

    /// @brief カメラのワールド行列を設定する
    /// @param worldMatrix ワールド行列
    void SetWorldMatrix(const Matrix4x4 &worldMatrix) noexcept;

    /// @brief 球面座標系を設定する
    /// @param sphericalCoordinateSystem 球面座標系
    void SetSphericalCoordinateSystem(const SphericalCoordinateSystem &sphericalCoordinateSystem) noexcept;

    /// @brief マウスでのカメラ操作
    /// @param translateSpeed 平行移動速度
    /// @param rotateSpeed 回転速度
    /// @param scaleSpeed 拡大縮小速度
    void MoveToMouse(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept;

    /// @brief ターゲット位置の設定
    /// @param targetPos ターゲット位置へのポインタ
    void Target(Vector3 *targetPos) noexcept;

    /// @brief カメラの拡大縮小ベクトルを取得する
    /// @return カメラの拡大縮小ベクトル
    [[nodiscard]] const Vector3 &GetScale() const noexcept {
        return cameraScale_;
    }

    /// @brief カメラの拡大縮小ベクトルへのポインタを取得する
    /// @return カメラの拡大縮小ベクトルへのポインタ
    [[nodiscard]] Vector3 *GetScalePtr() noexcept {
        return &cameraScale_;
    }

    /// @brief カメラの回転ベクトルを取得する
    /// @return カメラの回転ベクトル
    [[nodiscard]] const Vector3 &GetRotate() const noexcept {
        return cameraRotate_;
    }

    /// @brief カメラの回転ベクトルへのポインタを取得する
    /// @return カメラの回転ベクトルへのポインタ
    [[nodiscard]] Vector3 *GetRotatePtr() noexcept {
        return &cameraRotate_;
    }

    /// @brief カメラの平行移動ベクトルを取得する
    /// @return カメラの平行移動ベクトル
    [[nodiscard]] const Vector3 &GetTranslate() const noexcept {
        return cameraTranslate_;
    }

    /// @brief カメラの平行移動ベクトルへのポインタを取得する
    /// @return カメラの平行移動ベクトルへのポインタ
    [[nodiscard]] Vector3 *GetTranslatePtr() noexcept {
        return &cameraTranslate_;
    }

    /// @brief カメラ行列を取得する
    /// @return カメラ行列
    [[nodiscard]] const AffineMatrix &GetCameraMatrix() const noexcept {
        return cameraMatrix_;
    }

    /// @brief ビュー行列を取得する
    /// @return ビュー行列
    [[nodiscard]] const Matrix4x4 &GetViewMatrix() const noexcept {
        return viewMatrix_;
    }

    /// @brief 投影行列を取得する
    /// @return 投影行列
    [[nodiscard]] const Matrix4x4 &GetProjectionMatrix() const noexcept {
        return projectionMatrix_;
    }

    /// @brief ビュー投影行列を取得する
    /// @return ビュー投影行列
    [[nodiscard]] const Matrix4x4 &GetWVPMatrix() const noexcept {
        return wvpMatrix_;
    }

    /// @brief ビューポート行列を取得する
    /// @return ビューポート行列
    [[nodiscard]] const Matrix4x4 &GetViewportMatrix() const noexcept {
        return viewportMatrix_;
    }

private:
    /// @brief カメラの行列を計算する(デカルト座標系)
    void CalculateMatrixForDecart() noexcept;
    /// @brief カメラの行列を計算する(球面座標系)
    void CalculateMatrixForSpherical() noexcept;

    /// @brief マウスでのカメラ操作(デカルト座標系)
    /// @param translateSpeed 平行移動速度
    /// @param rotateSpeed 回転速度
    /// @param scaleSpeed 拡大縮小速度
    void MoveToMouseForDecart(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept;

    /// @brief マウスでのカメラ操作(球面座標系)
    /// @param translateSpeed 平行移動速度
    /// @param rotateSpeed 回転速度
    /// @param scaleSpeed 拡大縮小速度
    void MoveToMouseForSpherical(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept;

    CoordinateSystem coordinateSystem_;
    Vector3 *targetPos_;
    
    Vector3 cameraScale_;
    Vector3 cameraRotate_;
    Vector3 cameraTranslate_;
    
    AffineMatrix cameraMatrix_;
    Matrix4x4 worldMatrix_;
    Matrix4x4 viewMatrix_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 wvpMatrix_;
    Matrix4x4 viewportMatrix_;

    SphericalCoordinateSystem sphericalCoordinateSystem_;
};

} // namespace KashipanEngine