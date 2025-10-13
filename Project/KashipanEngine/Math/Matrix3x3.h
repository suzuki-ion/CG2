#pragma once

namespace KashipanEngine {

struct Vector2;

struct Matrix3x3 {
    Matrix3x3() noexcept = default;
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
    
    Matrix3x3 &operator+=(const Matrix3x3 &matrix) noexcept;
    Matrix3x3 &operator-=(const Matrix3x3 &matrix) noexcept;
    Matrix3x3 &operator*=(float scalar) noexcept;
    Matrix3x3 &operator*=(const Matrix3x3 &matrix) noexcept;
    constexpr Matrix3x3 operator+(const Matrix3x3 &matrix) const noexcept;
    constexpr Matrix3x3 operator-(const Matrix3x3 &matrix) const noexcept;
    constexpr Matrix3x3 operator*(float scalar) const noexcept;
    constexpr Matrix3x3 operator*(const Matrix3x3 &matrix) const noexcept;

    [[nodiscard]] static const Matrix3x3 Identity() noexcept;
    [[nodiscard]] const Matrix3x3 Transpose() const noexcept;
    [[nodiscard]] const float Determinant() const noexcept;
    [[nodiscard]] Matrix3x3 Inverse() const;

    void MakeIdentity() noexcept;
    void MakeTranspose() noexcept;
    void MakeInverse() noexcept;
    
    void MakeTranslate(const Vector2 &translate) noexcept;
    void MakeScale(const Vector2 &scale) noexcept;
    void MakeRotate(float radian) noexcept;
    void MakeAffine(const Vector2 &scale, float radian, const Vector2 &translate) noexcept;

    float m[3][3];
};

} // namespace KashipanEngine