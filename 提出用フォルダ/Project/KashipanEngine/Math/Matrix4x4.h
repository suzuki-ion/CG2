#pragma once

namespace KashipanEngine {

struct Vector3;

struct Matrix4x4 {
    // 大量に呼び出されるであろうデフォルトコンストラクタは軽量化のため何もしないようにしておく
    Matrix4x4() noexcept = default;
    constexpr Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept
        : m{
            {m00, m01, m02, m03},
            {m10, m11, m12, m13},
            {m20, m21, m22, m23},
            {m30, m31, m32, m33}
        } {
    }

    // 行列式計算用の2x2行列
    struct Matrix2x2 {
        constexpr Matrix2x2(float m00, float m01, float m10, float m11) noexcept
            : m{
                {m00, m01},
                {m10, m11}
            } {
        }
        /// @brief 行列式を計算する
        /// @return 行列式
        [[nodiscard]] constexpr float Determinant() const noexcept;
    private:
        float m[2][2];
    };

    // 行列式計算用の3x3行列
    struct Matrix3x3 {
        constexpr Matrix3x3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22) noexcept
            : m{
                {m00, m01, m02},
                {m10, m11, m12},
                {m20, m21, m22}
            } {
        }
        /// @brief 行列式を計算する
        /// @return 行列式
        [[nodiscard]] constexpr float Determinant() const noexcept;
    private:
        float m[3][3];
    };

    Matrix4x4 &operator+=(const Matrix4x4 &matrix) noexcept;
    Matrix4x4 &operator-=(const Matrix4x4 &matrix) noexcept;
    Matrix4x4 &operator*=(const float scalar) noexcept;
    Matrix4x4 &operator*=(const Matrix4x4 &matrix) noexcept;
    constexpr const Matrix4x4 operator+(const Matrix4x4 &matrix) const noexcept;
    constexpr const Matrix4x4 operator-(const Matrix4x4 &matrix) const noexcept;
    constexpr const Matrix4x4 operator*(const float scalar) const noexcept;
    constexpr const Matrix4x4 operator*(const Matrix4x4 &matrix) const noexcept;

    /// @brief 単位行列を取得する
    /// @return 単位行列
    [[nodiscard]] static const Matrix4x4 Identity() noexcept;

    /// @brief 転置行列を取得する
    /// @return 転置行列
    [[nodiscard]] const Matrix4x4 Transpose() noexcept;

    /// @brief 行列式を計算する
    /// @return 行列式
    [[nodiscard]] const float Determinant() const noexcept;

    /// @brief 逆行列を計算する
    /// @return 逆行列
    [[nodiscard]] Matrix4x4 Inverse() const;

    /// @brief 自身を単位行列にする
    void MakeIdentity() noexcept;

    /// @brief 自身を転置行列にする
    void MakeTranspose() noexcept;

    /// @brief 自身を逆行列にする
    void MakeInverse() noexcept;

    /// @brief 平行移動行列を生成する
    /// @param translate 平行移動ベクトル
    /// @return 平行移動行列
    void MakeTranslate(const Vector3 &translate) noexcept;

    /// @brief 拡大縮小行列を生成する
    /// @param scale 拡大縮小ベクトル
    /// @return 拡大縮小行列
    void MakeScale(const Vector3 &scale) noexcept;

    /// @brief 回転行列を生成する
    /// @param rotate 回転角度
    /// @return 回転行列
    void MakeRotate(const Vector3 &rotate) noexcept;

    /// @brief 回転行列を生成する
    /// @param radianX 回転角度 X
    /// @param radianY 回転角度 Y
    /// @param radianZ 回転角度 Z
    /// @return 回転行列
    void MakeRotate(
        const float radianX,
        const float radianY,
        const float radianZ) noexcept;

    /// @brief X軸回転行列を生成する
    /// @param radian 回転角度
    /// @return X軸回転行列
    void MakeRotateX(const float radian) noexcept;

    /// @brief Y軸回転行列を生成する
    /// @param radian 回転角度
    /// @return Y軸回転行列
    void MakeRotateY(const float radian) noexcept;

    /// @brief Z軸回転行列を生成する
    /// @param radian 回転角度
    /// @return Z軸回転行列
    void MakeRotateZ(const float radian) noexcept;

    /// @brief アフィン行列を生成する
    /// @param scale 拡大縮小ベクトル
    /// @param rotate 回転角度
    /// @param translate 平行移動ベクトル
    void MakeAffine(
        const Vector3 &scale,
        const Vector3 &rotate,
        const Vector3 &translate) noexcept;

    float m[4][4];
};

} // namespace KashipanEngine