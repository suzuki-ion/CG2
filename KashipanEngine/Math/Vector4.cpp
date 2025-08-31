#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace KashipanEngine {

Vector4 Vector4::Lerp(const Vector4 &start, const Vector4 &end, float t) noexcept {
    return start * (1.0f - t) + end * t;
}

Vector4::Vector4(const Vector2 &vector2) noexcept {
    x = vector2.x;
    y = vector2.y;
    z = 0.0f;
    w = 1.0f;
}

Vector4::Vector4(const Vector3 &vector3) noexcept {
    x = vector3.x;
    y = vector3.y;
    z = vector3.z;
    w = 1.0f;
}

Vector4 &Vector4::operator=(const Vector4 &vector) noexcept {
    x = vector.x;
    y = vector.y;
    z = vector.z;
    w = vector.w;
    return *this;
}

Vector4 &Vector4::operator+=(const Vector4 &vector) noexcept {
    x += vector.x;
    y += vector.y;
    z += vector.z;
    w += vector.w;
    return *this;
}

Vector4 &Vector4::operator-=(const Vector4 &vector) noexcept {
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    w -= vector.w;
    return *this;
}

Vector4 &Vector4::operator*=(const float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4 &Vector4::operator*=(const Vector4 &vector) noexcept {
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;
    w *= vector.w;
    return *this;
}

Vector4 &Vector4::operator/=(const float scalar) {
    if (scalar == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
    } else {
        float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
        w *= invScalar;
    }
    return *this;
}

Vector4 &Vector4::operator/=(const Vector4 &vector) {
    if (vector.x == 0.0f) {
        x = 0.0f;
    } else {
        x /= vector.x;
    }
    if (vector.y == 0.0f) {
        y = 0.0f;
    } else {
        y /= vector.y;
    }
    if (vector.z == 0.0f) {
        z = 0.0f;
    } else {
        z /= vector.z;
    }
    if (vector.w == 0.0f) {
        w = 0.0f;
    } else {
        w /= vector.w;
    }
    return *this;
}

bool Vector4::operator==(const Vector4 &vector) const noexcept {
    return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
}

bool Vector4::operator!=(const Vector4 &vector) const noexcept {
    return x != vector.x || y != vector.y || z != vector.z || w != vector.w;
}

} // namespace KashipanEngine