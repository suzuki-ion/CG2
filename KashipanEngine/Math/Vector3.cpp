#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <cassert>

namespace KashipanEngine {

Vector3::Vector3(const Vector4 &vector) {
    if (vector.w == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    } else {
        x = vector.x / vector.w;
        y = vector.y / vector.w;
        z = vector.z / vector.w;
    }
}

float Vector3::operator[](const int index) const noexcept {
    return (&x)[index];
}

float &Vector3::operator[](const int index) noexcept {
    return (&x)[index];
}

Vector3 &Vector3::operator=(const Vector3 &vector) noexcept {
    x = vector.x;
    y = vector.y;
    z = vector.z;
    return *this;
}

Vector3 &Vector3::operator+=(const Vector3 &vector) noexcept {
    x += vector.x;
    y += vector.y;
    z += vector.z;
    return *this;
}

Vector3 &Vector3::operator-=(const Vector3 &vector) noexcept {
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    return *this;
}

Vector3 &Vector3::operator*=(const float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 &Vector3::operator*=(const Vector3 &vector) noexcept {
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;
    return *this;
}

Vector3 &Vector3::operator/=(const float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 &Vector3::operator/=(const Vector3 &vector) {
    x /= vector.x;
    y /= vector.y;
    z /= vector.z;
    return *this;
}

bool Vector3::operator==(const Vector3 &vector) const noexcept {
    return x == vector.x && y == vector.y && z == vector.z;
}

bool Vector3::operator!=(const Vector3 &vector) const noexcept {
    return x != vector.x || y != vector.y || z != vector.z;
}

constexpr float Vector3::Dot(const Vector3 &vector) const noexcept {
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3 Vector3::Cross(const Vector3 &vector) const noexcept {
    return Vector3(
        y * vector.z - z * vector.y,
        z * vector.x - x * vector.z,
        x * vector.y - y * vector.x
    );
}

float Vector3::Length() const noexcept {
    return std::sqrt(LengthSquared());
}

constexpr float Vector3::LengthSquared() const noexcept {
    return Dot(*this);
}

Vector3 Vector3::Normalize() const {
    const float length = Length();
    if (length == 0.0f) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    return *this / length;
}

Vector3 Vector3::Projection(const Vector3 &vector) const noexcept {
    return (Dot(vector) / vector.Dot(vector)) * vector;
}

Vector3 Vector3::Perpendicular() const noexcept {
    if (x != 0.0f || y != 0.0f) {
        return Vector3(-y, x, 0.0f);
    }
    return Vector3(0.0f, -z, y);
}

inline constexpr const Vector3 Vector3::Rejection(const Vector3 &vector) const noexcept {
    return *this - Projection(vector);
}

inline constexpr const Vector3 Vector3::Refrection(const Vector3 &normal) const noexcept {
    return *this + (-2.0f * Dot(normal) * normal);
}

inline constexpr const Vector3 Vector3::Refrection(const Vector3 &normal, const float eta) const noexcept {
    const float cosTheta = Dot(normal);
    const float k = 1.0f - eta * eta * (1.0f - cosTheta * cosTheta);
    if (k < 0.0f) {
        return Vector3(0.0f);
    }
    return *this * eta + (eta * cosTheta - std::sqrt(k)) * normal;
}

float Vector3::Distance(const Vector3 &vector) const {
    return (vector - *this).Length();
}

Vector3 Vector3::Transform(const Matrix4x4 &mat) const noexcept {
    Vector3 result;
    result.x = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + 1.0f * mat.m[3][0];
    result.y = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + 1.0f * mat.m[3][1];
    result.z = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + 1.0f * mat.m[3][2];
    float w = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + 1.0f * mat.m[3][3];

    if (w == 0.0f) {
        return Vector3(0.0f);
    }

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}

const Vector3 operator*(const Matrix4x4 &mat, const Vector3 &vector) noexcept {
    return Vector3(
        mat.m[0][0] * vector.x + mat.m[0][1] * vector.y + mat.m[0][2] * vector.z + mat.m[0][3],
        mat.m[1][0] * vector.x + mat.m[1][1] * vector.y + mat.m[1][2] * vector.z + mat.m[1][3],
        mat.m[2][0] * vector.x + mat.m[2][1] * vector.y + mat.m[2][2] * vector.z + mat.m[2][3]
    );
}

const Vector3 operator*(const Vector3 &vector, const Matrix4x4 &mat) noexcept {
    return Vector3(
        vector.x * mat.m[0][0] + vector.y * mat.m[1][0] + vector.z * mat.m[2][0] + mat.m[3][0],
        vector.x * mat.m[0][1] + vector.y * mat.m[1][1] + vector.z * mat.m[2][1] + mat.m[3][1],
        vector.x * mat.m[0][2] + vector.y * mat.m[1][2] + vector.z * mat.m[2][2] + mat.m[3][2]
    );
}

} // namespace KashipanEngine