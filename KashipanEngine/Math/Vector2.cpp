#include "Vector2.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "MathObjects/Lines.h"
#include <cmath>

namespace KashipanEngine {

Vector2 Vector2::Lerp(const Vector2 &start, const Vector2 &end, float t) noexcept {
    return t * start + (1.0f - t) * end;
}

Vector2::Vector2(const Vector3 &vector) {
    x = vector.x;
    y = vector.y;
}

Vector2 &Vector2::operator=(const Vector2 &vector) {
    x = vector.x;
    y = vector.y;
    return *this;
}

Vector2 &Vector2::operator+=(const Vector2 &vector) noexcept {
    x += vector.x;
    y += vector.y;
    return *this;
}

Vector2 &Vector2::operator-=(const Vector2 &vector) noexcept {
    x -= vector.x;
    y -= vector.y;
    return *this;
}

Vector2 &Vector2::operator*=(float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 &Vector2::operator*=(const Vector2 &vector) noexcept {
    x *= vector.x;
    y *= vector.y;
    return *this;
}

Vector2 &Vector2::operator/=(float scalar) {
    if (scalar == 0.0f) {
        x = 0.0f;
        y = 0.0f;
    } else {
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
    }
    return *this;
}

Vector2 &Vector2::operator/=(const Vector2 &vector) {
    x = (vector.x != 0.0f) ? x / vector.x : 0.0f;
    y = (vector.y != 0.0f) ? y / vector.y : 0.0f;
    return *this;
}

bool Vector2::operator==(const Vector2 &vector) const noexcept {
    return x == vector.x && y == vector.y;
}

bool Vector2::operator!=(const Vector2 &vector) const noexcept {
    return x != vector.x || y != vector.y;
}

constexpr float Vector2::Dot(const Vector2 &vector) const noexcept {
    return x * vector.x + y * vector.y;
}

constexpr float Vector2::Cross(const Vector2 &vector) const noexcept {
    return x * vector.y - y * vector.x;
}

float Vector2::Length() const noexcept {
    return std::sqrt(LengthSquared());
}

constexpr float Vector2::LengthSquared() const noexcept {
    return Dot(*this);
}

Vector2 Vector2::Normalize() const {
    const float len = Length();
    return (len != 0.0f) ? *this / len : Vector2(0.0f);
}

Vector2 Vector2::Projection(const Vector2 &vector) const noexcept {
    const float denom = vector.Dot(vector);
    return (denom != 0.0f) ? (Dot(vector) / denom) * vector : Vector2(0.0f);
}

Vector2 Vector2::Rejection(const Vector2 &vector) const noexcept {
    return *this - Projection(vector);
}

Vector2 Vector2::Perpendicular() const noexcept {
    return Vector2(-y, x);
}

Vector2 Vector2::Refrection(const Vector2 &normal) const noexcept {
    return *this - 2.0f * Dot(normal) * normal;
}

float Vector2::Distance(const Vector2 &vector) const noexcept {
    return (*this - vector).Length();
}

Vector2 Vector2::ClosestPoint(const Math::Segment &segment) const noexcept {
    Vector2 origin(segment.origin);
    Vector2 diff(segment.diff);
    return origin + (*this - origin).Projection(diff);
}

inline constexpr Vector2 operator*(const Matrix3x3 &matrix, const Vector2 &vector) noexcept {
    return Vector2(
        matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0],
        matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1]
    );
}

inline constexpr Vector2 operator*(const Vector2 &vector, const Matrix3x3 &matrix) noexcept {
    return Vector2(
        vector.x * matrix.m[0][0] + vector.y * matrix.m[0][1] + matrix.m[0][2],
        vector.x * matrix.m[1][0] + vector.y * matrix.m[1][1] + matrix.m[1][2]
    );
}

} // namespace KashipanEngine