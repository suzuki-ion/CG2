#include "Matrix4x4.h"
#include "Vector3.h"

namespace KashipanEngine {

constexpr float Matrix4x4::Matrix2x2::Determinant() const noexcept {
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

constexpr float Matrix4x4::Matrix3x3::Determinant() const noexcept {
    float c00 = Matrix2x2(m[1][1], m[1][2], m[2][1], m[2][2]).Determinant();
    float c01 = -(Matrix2x2(m[1][0], m[1][2], m[2][0], m[2][2]).Determinant());
    float c02 = Matrix2x2(m[1][0], m[1][1], m[2][0], m[2][1]).Determinant();
    return m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02;
}

Matrix4x4 &Matrix4x4::operator+=(const Matrix4x4 &matrix) noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    m[0][0] += matrix.m[0][0];
    m[0][1] += matrix.m[0][1];
    m[0][2] += matrix.m[0][2];
    m[0][3] += matrix.m[0][3];

    m[1][0] += matrix.m[1][0];
    m[1][1] += matrix.m[1][1];
    m[1][2] += matrix.m[1][2];
    m[1][3] += matrix.m[1][3];

    m[2][0] += matrix.m[2][0];
    m[2][1] += matrix.m[2][1];
    m[2][2] += matrix.m[2][2];
    m[2][3] += matrix.m[2][3];

    m[3][0] += matrix.m[3][0];
    m[3][1] += matrix.m[3][1];
    m[3][2] += matrix.m[3][2];
    m[3][3] += matrix.m[3][3];

    return *this;
}

Matrix4x4 &Matrix4x4::operator-=(const Matrix4x4 &matrix) noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    m[0][0] -= matrix.m[0][0];
    m[0][1] -= matrix.m[0][1];
    m[0][2] -= matrix.m[0][2];
    m[0][3] -= matrix.m[0][3];

    m[1][0] -= matrix.m[1][0];
    m[1][1] -= matrix.m[1][1];
    m[1][2] -= matrix.m[1][2];
    m[1][3] -= matrix.m[1][3];

    m[2][0] -= matrix.m[2][0];
    m[2][1] -= matrix.m[2][1];
    m[2][2] -= matrix.m[2][2];
    m[2][3] -= matrix.m[2][3];

    m[3][0] -= matrix.m[3][0];
    m[3][1] -= matrix.m[3][1];
    m[3][2] -= matrix.m[3][2];
    m[3][3] -= matrix.m[3][3];

    return *this;
}

Matrix4x4 &Matrix4x4::operator*=(const float scalar) noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    m[0][0] *= scalar;
    m[0][1] *= scalar;
    m[0][2] *= scalar;
    m[0][3] *= scalar;

    m[1][0] *= scalar;
    m[1][1] *= scalar;
    m[1][2] *= scalar;
    m[1][3] *= scalar;

    m[2][0] *= scalar;
    m[2][1] *= scalar;
    m[2][2] *= scalar;
    m[2][3] *= scalar;

    m[3][0] *= scalar;
    m[3][1] *= scalar;
    m[3][2] *= scalar;
    m[3][3] *= scalar;

    return *this;
}

Matrix4x4 &Matrix4x4::operator*=(const Matrix4x4 &matrix) noexcept {
    *this = *this * matrix;
    return *this;
}

constexpr const Matrix4x4 Matrix4x4::operator+(const Matrix4x4 &matrix) const noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    return Matrix4x4(
        m[0][0] + matrix.m[0][0], m[0][1] + matrix.m[0][1], m[0][2] + matrix.m[0][2], m[0][3] + matrix.m[0][3],
        m[1][0] + matrix.m[1][0], m[1][1] + matrix.m[1][1], m[1][2] + matrix.m[1][2], m[1][3] + matrix.m[1][3],
        m[2][0] + matrix.m[2][0], m[2][1] + matrix.m[2][1], m[2][2] + matrix.m[2][2], m[2][3] + matrix.m[2][3],
        m[3][0] + matrix.m[3][0], m[3][1] + matrix.m[3][1], m[3][2] + matrix.m[3][2], m[3][3] + matrix.m[3][3]
    );
}

constexpr const Matrix4x4 Matrix4x4::operator-(const Matrix4x4 &matrix) const noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    return Matrix4x4(
        m[0][0] - matrix.m[0][0], m[0][1] - matrix.m[0][1], m[0][2] - matrix.m[0][2], m[0][3] - matrix.m[0][3],
        m[1][0] - matrix.m[1][0], m[1][1] - matrix.m[1][1], m[1][2] - matrix.m[1][2], m[1][3] - matrix.m[1][3],
        m[2][0] - matrix.m[2][0], m[2][1] - matrix.m[2][1], m[2][2] - matrix.m[2][2], m[2][3] - matrix.m[2][3],
        m[3][0] - matrix.m[3][0], m[3][1] - matrix.m[3][1], m[3][2] - matrix.m[3][2], m[3][3] - matrix.m[3][3]
    );
}

constexpr const Matrix4x4 Matrix4x4::operator*(const float scalar) const noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    return Matrix4x4(
        m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar, m[0][3] * scalar,
        m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar, m[1][3] * scalar,
        m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar, m[2][3] * scalar,
        m[3][0] * scalar, m[3][1] * scalar, m[3][2] * scalar, m[3][3] * scalar
    );
}

constexpr const Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &matrix) const noexcept {
    // 少しでも速度を稼ぐためにループではなく展開する
    return Matrix4x4(
        m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0],
        m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1] + m[0][3] * matrix.m[3][1],
        m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2] + m[0][3] * matrix.m[3][2],
        m[0][0] * matrix.m[0][3] + m[0][1] * matrix.m[1][3] + m[0][2] * matrix.m[2][3] + m[0][3] * matrix.m[3][3],
        m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0] + m[1][3] * matrix.m[3][0],
        m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1] + m[1][3] * matrix.m[3][1],
        m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2] + m[1][3] * matrix.m[3][2],
        m[1][0] * matrix.m[0][3] + m[1][1] * matrix.m[1][3] + m[1][2] * matrix.m[2][3] + m[1][3] * matrix.m[3][3],
        m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0] + m[2][3] * matrix.m[3][0],
        m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1] + m[2][3] * matrix.m[3][1],
        m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2] + m[2][3] * matrix.m[3][2],
        m[2][0] * matrix.m[0][3] + m[2][1] * matrix.m[1][3] + m[2][2] * matrix.m[2][3] + m[2][3] * matrix.m[3][3],
        m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0] + m[3][3] * matrix.m[3][0],
        m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1] + m[3][3] * matrix.m[3][1],
        m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2] + m[3][3] * matrix.m[3][2],
        m[3][0] * matrix.m[0][3] + m[3][1] * matrix.m[1][3] + m[3][2] * matrix.m[2][3] + m[3][3] * matrix.m[3][3]
    );
}

const Matrix4x4 Matrix4x4::Identity() noexcept {
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

const Matrix4x4 Matrix4x4::Transpose() noexcept {
    return Matrix4x4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

const float Matrix4x4::Determinant() const noexcept {
    float c00 = Matrix3x3(
        m[1][1], m[1][2], m[1][3],
        m[2][1], m[2][2], m[2][3],
        m[3][1], m[3][2], m[3][3]).Determinant();
    float c01 = -(Matrix3x3(
        m[1][0], m[1][2], m[1][3],
        m[2][0], m[2][2], m[2][3],
        m[3][0], m[3][2], m[3][3]).Determinant());
    float c02 = Matrix3x3(
        m[1][0], m[1][1], m[1][3],
        m[2][0], m[2][1], m[2][3],
        m[3][0], m[3][1], m[3][3]).Determinant();
    float c03 = -(Matrix3x3(
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2],
        m[3][0], m[3][1], m[3][2]).Determinant());
    return (m[0][0] * c00) + (m[0][1] * c01) + (m[0][2] * c02) + (m[0][3] * c03);
}

Matrix4x4 Matrix4x4::Inverse() const {
    float c00 = Matrix3x3(
        m[1][1], m[1][2], m[1][3],
        m[2][1], m[2][2], m[2][3],
        m[3][1], m[3][2], m[3][3]).Determinant();
    float c01 = -(Matrix3x3(
        m[1][0], m[1][2], m[1][3],
        m[2][0], m[2][2], m[2][3],
        m[3][0], m[3][2], m[3][3]).Determinant());
    float c02 = Matrix3x3(
        m[1][0], m[1][1], m[1][3],
        m[2][0], m[2][1], m[2][3],
        m[3][0], m[3][1], m[3][3]).Determinant();
    float c03 = -(Matrix3x3(
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2],
        m[3][0], m[3][1], m[3][2]).Determinant());

    float c10 = -(Matrix3x3(
        m[0][1], m[0][2], m[0][3],
        m[2][1], m[2][2], m[2][3],
        m[3][1], m[3][2], m[3][3]).Determinant());
    float c11 = Matrix3x3(
        m[0][0], m[0][2], m[0][3],
        m[2][0], m[2][2], m[2][3],
        m[3][0], m[3][2], m[3][3]).Determinant();
    float c12 = -(Matrix3x3(
        m[0][0], m[0][1], m[0][3],
        m[2][0], m[2][1], m[2][3],
        m[3][0], m[3][1], m[3][3]).Determinant());
    float c13 = Matrix3x3(
        m[0][0], m[0][1], m[0][2],
        m[2][0], m[2][1], m[2][2],
        m[3][0], m[3][1], m[3][2]).Determinant();

    float c20 = Matrix3x3(
        m[0][1], m[0][2], m[0][3],
        m[1][1], m[1][2], m[1][3],
        m[3][1], m[3][2], m[3][3]).Determinant();
    float c21 = -(Matrix3x3(
        m[0][0], m[0][2], m[0][3],
        m[1][0], m[1][2], m[1][3],
        m[3][0], m[3][2], m[3][3]).Determinant());
    float c22 = Matrix3x3(
        m[0][0], m[0][1], m[0][3],
        m[1][0], m[1][1], m[1][3],
        m[3][0], m[3][1], m[3][3]).Determinant();
    float c23 = -(Matrix3x3(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[3][0], m[3][1], m[3][2]).Determinant());

    float c30 = -(Matrix3x3(
        m[0][1], m[0][2], m[0][3],
        m[1][1], m[1][2], m[1][3],
        m[2][1], m[2][2], m[2][3]).Determinant());
    float c31 = Matrix3x3(
        m[0][0], m[0][2], m[0][3],
        m[1][0], m[1][2], m[1][3],
        m[2][0], m[2][2], m[2][3]).Determinant();
    float c32 = -(Matrix3x3(
        m[0][0], m[0][1], m[0][3],
        m[1][0], m[1][1], m[1][3],
        m[2][0], m[2][1], m[2][3]).Determinant());
    float c33 = Matrix3x3(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]).Determinant();

    float det = 1.0f / (m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02 + m[0][3] * c03);

    return Matrix4x4(
        c00 * det, c10 * det, c20 * det, c30 * det,
        c01 * det, c11 * det, c21 * det, c31 * det,
        c02 * det, c12 * det, c22 * det, c32 * det,
        c03 * det, c13 * det, c23 * det, c33 * det
    );
}

void Matrix4x4::MakeIdentity() noexcept {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeTranspose() noexcept {
    float tmp;
    tmp = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = tmp;
    tmp = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = tmp;
    tmp = m[0][3];
    m[0][3] = m[3][0];
    m[3][0] = tmp;
    tmp = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = tmp;
    tmp = m[1][3];
    m[1][3] = m[3][1];
    m[3][1] = tmp;
    tmp = m[2][3];
    m[2][3] = m[3][2];
    m[3][2] = tmp;
}

void Matrix4x4::MakeInverse() noexcept {
    *this = Inverse();
}

void Matrix4x4::MakeTranslate(const Vector3 &translate) noexcept {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = translate.x;
    m[3][1] = translate.y;
    m[3][2] = translate.z;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeScale(const Vector3 &scale) noexcept {
    m[0][0] = scale.x;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = scale.y;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = scale.z;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeRotate(const Vector3 &rotate) noexcept {
    Matrix4x4 mX{};
    Matrix4x4 mY{};
    Matrix4x4 mZ{};
    mX.MakeRotateX(rotate.x);
    mY.MakeRotateY(rotate.y);
    mZ.MakeRotateZ(rotate.z);
    *this = mX * mY * mZ;
}

void Matrix4x4::MakeRotate(const float radianX, const float radianY, const float radianZ) noexcept {
    Matrix4x4 mX{};
    Matrix4x4 mY{};
    Matrix4x4 mZ{};
    mX.MakeRotateX(radianX);
    mY.MakeRotateY(radianY);
    mZ.MakeRotateZ(radianZ);
    *this = mX * mY * mZ;
}

void Matrix4x4::MakeRotateX(const float radian) noexcept {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = std::cos(radian);
    m[1][2] = std::sin(radian);
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = -std::sin(radian);
    m[2][2] = std::cos(radian);
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeRotateY(const float radian) noexcept {
    m[0][0] = std::cos(radian);
    m[0][1] = 0.0f;
    m[0][2] = -std::sin(radian);
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = std::sin(radian);
    m[2][1] = 0.0f;
    m[2][2] = std::cos(radian);
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeRotateZ(const float radian) noexcept {
    m[0][0] = std::cos(radian);
    m[0][1] = std::sin(radian);
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = -std::sin(radian);
    m[1][1] = std::cos(radian);
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void Matrix4x4::MakeAffine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate) noexcept {
    Matrix4x4 mX{};
    Matrix4x4 mY{};
    Matrix4x4 mZ{};
    Matrix4x4 mT{};
    Matrix4x4 mS{};
    mX.MakeRotateX(rotate.x);
    mY.MakeRotateY(rotate.y);
    mZ.MakeRotateZ(rotate.z);
    mT.MakeTranslate(translate);
    mS.MakeScale(scale);
    *this = mS * (mX * mY * mZ) * mT;
}

} // namespace KashipanEngine