#include "Vector2.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "MathObjects/Lines.h"
#include <cassert>
#include <algorithm>

namespace KashipanEngine {

Vector2 Vector2::Lerp(const Vector2 &start, const Vector2 &end, float t) noexcept {
    return t * start + (1.0f - t) * end;
}

Vector2 Vector2::Slerp(const Vector2 &start, const Vector2 &end, float t) noexcept {
    Vector2 normalizedStart = start.Normalize();
    Vector2 normalizedEnd = end.Normalize();
    float angle = std::acos(normalizedStart.Dot(normalizedEnd));
    float sinTheta = std::sin(angle);

    float t1 = std::sin(angle * (1.0f - t));
    float t2 = std::sin(angle * t);

    Vector2 result = (normalizedStart * t1 + normalizedEnd * t2) / sinTheta;
    return result.Normalize();
}

Vector2 Vector2::Bezier(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, float t) noexcept {
    Vector2 p01 = Vector2::Lerp(p0, p1, t);
    Vector2 p12 = Vector2::Lerp(p1, p2, t);
    return Vector2::Lerp(p01, p12, t);
}

Vector2 Vector2::CatmullRomInterpolation(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, float t) noexcept {
    const float s = 0.5f;

    float t2 = t * t;
    float t3 = t2 * t;

    auto e3 = (-p0 + (3.0f * p1) - (3.0f * p2) + p3) * t3;
    auto e2 = ((2.0f * p0) - (5.0f * p1) + (4.0f * p2) - p3) * t2;
    auto e1 = (-p0 + p2) * t;
    auto e0 = 2.0f * p1;

    return s * (e3 + e2 + e1 + e0);
}

Vector2 Vector2::CatmullRomPosition(const std::vector<Vector2> &points, float t, bool isLoop) {
    assert(points.size() >= 4);

    size_t division = isLoop ? points.size() : points.size() - 1;
    float areaWidth = 1.0f / static_cast<float>(division);
    size_t index    = static_cast<size_t>(t / areaWidth);
    index           = std::min(index, division - 1);
    float t2        = (t - areaWidth * static_cast<float>(index)) / areaWidth;
    t2              = std::clamp(t2, 0.0f, 1.0f);

    // インデックス取得関数（ループ対応）
    auto GetIndex = [&](int i) -> size_t {
        if (isLoop) {
            return (i + points.size()) % points.size();
        } else {
            return std::clamp(i, 0, static_cast<int>(points.size()) - 1);
        }
        };

    size_t index0 = GetIndex(static_cast<int>(index) - 1);
    size_t index1 = GetIndex(static_cast<int>(index));
    size_t index2 = GetIndex(static_cast<int>(index) + 1);
    size_t index3 = GetIndex(static_cast<int>(index) + 2);

    const auto &p0 = points[index0];
    const auto &p1 = points[index1];
    const auto &p2 = points[index2];
    const auto &p3 = points[index3];

    return CatmullRomInterpolation(p0, p1, p2, p3, t2);
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