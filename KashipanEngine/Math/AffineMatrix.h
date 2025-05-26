#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

namespace KashipanEngine {

class AffineMatrix {
public:
    AffineMatrix() noexcept = default;
    AffineMatrix(const Matrix4x4 &scale, const Matrix4x4 &rotate, const Matrix4x4 &translate) noexcept;
    AffineMatrix(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate) noexcept;
    AffineMatrix(const AffineMatrix &affine) noexcept;

    AffineMatrix &operator=(const AffineMatrix &affine) noexcept;
    AffineMatrix &operator*=(const AffineMatrix &affine) noexcept;
    AffineMatrix &operator*=(const Matrix4x4 &mat) noexcept;

    /// @brief 拡大縮小行列、回転行列、平行移動行列を設定する
    /// @param scale 拡大縮小ベクトル
    /// @param rotate 回転ベクトル
    /// @param translate 平行移動ベクトル
    void SetSRT(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);

    /// @brief 拡大縮小行列を設定する
    /// @param scale 拡大縮小ベクトル
    void SetScale(const Vector3 &scale) noexcept;

    /// @brief 拡大縮小行列を設定する
    /// @param scale 拡大縮小行列
    void SetScale(const Matrix4x4 &scale) noexcept;

    /// @brief 拡大縮小行列を設定する
    /// @param scaleX X軸方向の拡大縮小
    /// @param scaleY Y軸方向の拡大縮小
    /// @param scaleZ Z軸方向の拡大縮小
    void SetScale(float scaleX, float scaleY, float scaleZ) noexcept;

    /// @brief 拡大縮小行列を設定する
    /// @param scale 拡大縮小
    void SetScale(float scale) noexcept;

    /// @brief 回転行列を設定する
    /// @param rotate 回転ベクトル
    void SetRotate(const Vector3 &rotate) noexcept;

    /// @brief 回転行列を設定する
    /// @param rotate 回転行列
    void SetRotate(const Matrix4x4 &rotate) noexcept;

    /// @brief 回転行列を設定する
    /// @param radianX X軸方向の回転角度
    /// @param radianY Y軸方向の回転角度
    /// @param radianZ Z軸方向の回転角度
    void SetRotate(float radianX, float radianY, float radianZ) noexcept;

    /// @brief 平行移動行列を設定する
    /// @param translate 平行移動ベクトル
    void SetTranslate(const Vector3 &translate) noexcept;

    /// @brief 平行移動行列を設定する
    /// @param translate 平行移動行列
    void SetTranslate(const Matrix4x4 &translate) noexcept;

    /// @brief 平行移動行列を設定する
    /// @param translateX X軸方向の平行移動
    /// @param translateY Y軸方向の平行移動
    /// @param translateZ Z軸方向の平行移動
    void SetTranslate(float translateX, float translateY, float translateZ) noexcept;

    /// @brief 拡大縮小行列の逆行列を計算する
    /// @return 拡大縮小行列の逆行列
    [[nodiscard]] Matrix4x4 InverseScale() const noexcept;

    /// @brief 回転行列の逆行列を計算する
    /// @return 回転行列の逆行列
    [[nodiscard]] Matrix4x4 InverseRotate() const noexcept;

    /// @brief 平行移動行列の逆行列を計算する
    /// @return 平行移動行列の逆行列
    [[nodiscard]] Matrix4x4 InverseTranslate() const noexcept;

    /// @brief 拡大縮小行列を取得する
    /// @return 拡大縮小行列
    [[nodiscard]] const Matrix4x4 &GetScaleMatrix() const noexcept {
        return scaleMatrix;
    }

    /// @brief 回転行列を取得する
    /// @return 回転行列
    [[nodiscard]] const Matrix4x4 &GetRotateMatrix() const noexcept {
        return rotateMatrix;
    }

    /// @brief 平行移動行列を取得する
    /// @return 平行移動行列
    [[nodiscard]] const Matrix4x4 &GetTranslateMatrix() const noexcept {
        return translateMatrix;
    }

    /// @brief ワールド行列を取得する
    /// @return ワールド行列
    [[nodiscard]] const Matrix4x4 &GetWorldMatrix() const noexcept {
        return worldMatrix;
    }

private:
    Matrix4x4 scaleMatrix;
    Matrix4x4 rotateMatrix;
    Matrix4x4 translateMatrix;
    Matrix4x4 worldMatrix;
};

} // namespace KashipanEngine