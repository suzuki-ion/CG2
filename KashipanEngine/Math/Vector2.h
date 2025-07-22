#pragma once
#include <stdexcept>
#include <cmath>
#include <vector>

namespace KashipanEngine {

struct Vector3;
struct Matrix3x3;

namespace Math {
struct Segment;
} // namespace Math

struct Vector2 {
    static Vector2 Lerp(const Vector2 &start, const Vector2 &end, float t) noexcept;
    static Vector2 Slerp(const Vector2 &start, const Vector2 &end, float t) noexcept;
    static Vector2 Bezier(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, float t) noexcept;
    static Vector2 CatmullRomInterpolation(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, float t) noexcept;
    static Vector2 CatmullRomPosition(const std::vector<Vector2> &points, float t);

    Vector2() noexcept = default;
    constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}
    explicit constexpr Vector2(float v) noexcept : x(v), y(v) {}
    Vector2(const Vector2 &vector) : x(vector.x), y(vector.y) {}
    Vector2(const Vector3 &vector);

    Vector2 &operator=(const Vector2 &vector);
    Vector2 &operator+=(const Vector2 &vector) noexcept;
    Vector2 &operator-=(const Vector2 &vector) noexcept;
    Vector2 &operator*=(float scalar) noexcept;
    Vector2 &operator*=(const Vector2 &vector) noexcept;
    Vector2 &operator/=(float scalar);
    Vector2 &operator/=(const Vector2 &vector);
    bool operator==(const Vector2 &vector) const noexcept;
    bool operator!=(const Vector2 &vector) const noexcept;

    [[nodiscard]] constexpr float Dot(const Vector2 &vector) const noexcept;
    [[nodiscard]] constexpr float Cross(const Vector2 &vector) const noexcept;
    [[nodiscard]] float Length() const noexcept;
    [[nodiscard]] constexpr float LengthSquared() const noexcept;
    [[nodiscard]] Vector2 Normalize() const;
    [[nodiscard]] Vector2 Projection(const Vector2 &vector) const noexcept;
    [[nodiscard]] Vector2 ClosestPoint(const Math::Segment &segment) const noexcept;
    [[nodiscard]] Vector2 Perpendicular() const noexcept;
    [[nodiscard]] Vector2 Rejection(const Vector2 &vector) const noexcept;
    [[nodiscard]] Vector2 Refrection(const Vector2 &normal) const noexcept;
    [[nodiscard]] float Distance(const Vector2 &vector) const noexcept;

    float x;
    float y;
};

inline constexpr Vector2 operator-(const Vector2 &vector) noexcept {
    return Vector2(-vector.x, -vector.y);
}

inline constexpr Vector2 operator+(const Vector2 &a, const Vector2 &b) noexcept {
    return Vector2(a.x + b.x, a.y + b.y);
}

inline constexpr Vector2 operator-(const Vector2 &a, const Vector2 &b) noexcept {
    return Vector2(a.x - b.x, a.y - b.y);
}

inline constexpr Vector2 operator*(const Vector2 &vector, float scalar) noexcept {
    return Vector2(vector.x * scalar, vector.y * scalar);
}

inline constexpr Vector2 operator*(float scalar, const Vector2 &vector) noexcept {
    return Vector2(vector.x * scalar, vector.y * scalar);
}

inline constexpr Vector2 operator/(const Vector2 &vector, float scalar) {
    return vector * (1.0f / scalar);
}

inline constexpr Vector2 operator/(const Vector2 &a, const Vector2 &b) {
    return Vector2(a.x / b.x, a.y / b.y);
}

inline constexpr Vector2 operator*(const Matrix3x3 &matrix, const Vector2 &vector) noexcept;
inline constexpr Vector2 operator*(const Vector2 &vector, const Matrix3x3 &matrix) noexcept;

} // namespace KashipanEngine