#include "Matrix3x3.h"
#include "Vector2.h"

namespace KashipanEngine {

constexpr float Matrix3x3::Matrix2x2::Determinant() const noexcept {
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}


Matrix3x3 &Matrix3x3::operator+=(const Matrix3x3 &matrix) noexcept {
    m[0][0] += matrix.m[0][0];
    m[0][1] += matrix.m[0][1];
    m[0][2] += matrix.m[0][2];

    m[1][0] += matrix.m[1][0];
    m[1][1] += matrix.m[1][1];
    m[1][2] += matrix.m[1][2];

    m[2][0] += matrix.m[2][0];
    m[2][1] += matrix.m[2][1];
    m[2][2] += matrix.m[2][2];

    return *this;
}

Matrix3x3 &Matrix3x3::operator-=(const Matrix3x3 &matrix) noexcept {
    m[0][0] -= matrix.m[0][0];
    m[0][1] -= matrix.m[0][1];
    m[0][2] -= matrix.m[0][2];

    m[1][0] -= matrix.m[1][0];
    m[1][1] -= matrix.m[1][1];
    m[1][2] -= matrix.m[1][2];

    m[2][0] -= matrix.m[2][0];
    m[2][1] -= matrix.m[2][1];
    m[2][2] -= matrix.m[2][2];

    return *this;
}

Matrix3x3 &Matrix3x3::operator*=(const float scalar) noexcept {
    m[0][0] *= scalar;
    m[0][1] *= scalar;
    m[0][2] *= scalar;

    m[1][0] *= scalar;
    m[1][1] *= scalar;
    m[1][2] *= scalar;

    m[2][0] *= scalar;
    m[2][1] *= scalar;
    m[2][2] *= scalar;

    return *this;
}

Matrix3x3 &Matrix3x3::operator*=(const Matrix3x3 &matrix) noexcept {
    *this = *this * matrix;
    return *this;
}

constexpr Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &matrix) const noexcept {
    return Matrix3x3(
        m[0][0] + matrix.m[0][0], m[0][1] + matrix.m[0][1], m[0][2] + matrix.m[0][2],
        m[1][0] + matrix.m[1][0], m[1][1] + matrix.m[1][1], m[1][2] + matrix.m[1][2],
        m[2][0] + matrix.m[2][0], m[2][1] + matrix.m[2][1], m[2][2] + matrix.m[2][2]
    );
}

constexpr Matrix3x3 Matrix3x3::operator-(const Matrix3x3 &matrix) const noexcept {
    return Matrix3x3(
        m[0][0] - matrix.m[0][0], m[0][1] - matrix.m[0][1], m[0][2] - matrix.m[0][2],
        m[1][0] - matrix.m[1][0], m[1][1] - matrix.m[1][1], m[1][2] - matrix.m[1][2],
        m[2][0] - matrix.m[2][0], m[2][1] - matrix.m[2][1], m[2][2] - matrix.m[2][2]
    );
}

constexpr Matrix3x3 Matrix3x3::operator*(float scalar) const noexcept {
    return Matrix3x3(
        m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar,
        m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar,
        m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar
    );
}

constexpr Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &matrix) const noexcept {
    return Matrix3x3(
        m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0],
        m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1],
        m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2],

        m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0],
        m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1],
        m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2],

        m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0],
        m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1],
        m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2]
    );
}

const Matrix3x3 Matrix3x3::Identity() noexcept {
    return Matrix3x3(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );
}

const Matrix3x3 Matrix3x3::Transpose() const noexcept {
    return Matrix3x3(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
    );
}

const float Matrix3x3::Determinant() const noexcept {
    float c00 = Matrix2x2(m[1][1], m[1][2], m[2][1], m[2][2]).Determinant();
    float c01 = -(Matrix2x2(m[1][0], m[1][2], m[2][0], m[2][2]).Determinant());
    float c02 = Matrix2x2(m[1][0], m[1][1], m[2][0], m[2][1]).Determinant();
    return m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02;
}

Matrix3x3 Matrix3x3::Inverse() const {
    using M2 = Matrix3x3::Matrix2x2;

    float c00 = M2(m[1][1], m[1][2], m[2][1], m[2][2]).Determinant();
    float c01 = -M2(m[1][0], m[1][2], m[2][0], m[2][2]).Determinant();
    float c02 = M2(m[1][0], m[1][1], m[2][0], m[2][1]).Determinant();

    float c10 = -M2(m[0][1], m[0][2], m[2][1], m[2][2]).Determinant();
    float c11 = M2(m[0][0], m[0][2], m[2][0], m[2][2]).Determinant();
    float c12 = -M2(m[0][0], m[0][1], m[2][0], m[2][1]).Determinant();

    float c20 = M2(m[0][1], m[0][2], m[1][1], m[1][2]).Determinant();
    float c21 = -M2(m[0][0], m[0][2], m[1][0], m[1][2]).Determinant();
    float c22 = M2(m[0][0], m[0][1], m[1][0], m[1][1]).Determinant();

    float det = m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02;
    if (det == 0.0f) {
        // 非正則の場合は単位行列を返す
        return Matrix3x3::Identity();
    }
    float invDet = 1.0f / det;

    return Matrix3x3(
        c00 * invDet, c10 * invDet, c20 * invDet,
        c01 * invDet, c11 * invDet, c21 * invDet,
        c02 * invDet, c12 * invDet, c22 * invDet
    );
}

void Matrix3x3::MakeIdentity() noexcept {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
}

void Matrix3x3::MakeTranspose() noexcept {
    float tmp;
    tmp = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = tmp;
    tmp = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = tmp;
    tmp = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = tmp;
}

void Matrix3x3::MakeInverse() noexcept {
    *this = Inverse();
}

void Matrix3x3::MakeTranslate(const Vector2 &translate) noexcept {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;

    m[2][0] = translate.x;
    m[2][1] = translate.y;
    m[2][2] = 1.0f;
}

void Matrix3x3::MakeScale(const Vector2 &scale) noexcept {
    m[0][0] = scale.x;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    
    m[1][0] = 0.0f;
    m[1][1] = scale.y;
    m[1][2] = 0.0f;
    
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
}

void Matrix3x3::MakeRotate(float radian) noexcept {
    float c = std::cos(radian);
    float s = std::sin(radian);

    m[0][0] = c;
    m[0][1] = s;
    m[0][2] = 0.0f;
    
    m[1][0] = -s;
    m[1][1] = c;
    m[1][2] = 0.0f;
    
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
}

void Matrix3x3::MakeAffine(const Vector2 &scale, float radian, const Vector2 &translate) noexcept {
    Matrix3x3 s;
    Matrix3x3 r;
    Matrix3x3 t;
    s.MakeScale(scale);
    r.MakeRotate(radian);
    t.MakeTranslate(translate);
    *this = s * r * t;
}

} // namespace KashipanEngine