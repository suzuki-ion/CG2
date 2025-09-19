#pragma once
#include <stdexcept>
#include <cmath>
#include <vector>

namespace KashipanEngine {

struct Vector2;
struct Vector4;
struct Matrix4x4;

namespace Math {
struct Segment;
} // namespace Math

struct Vector3 {
    static Vector3 Lerp(const Vector3 &start, const Vector3 &end, float t) noexcept;
    static Vector3 Slerp(const Vector3 &start, const Vector3 &end, float t) noexcept;
    static Vector3 Bezier(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const float t) noexcept;
    static Vector3 CatmullRomInterpolation(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, float t) noexcept;
    static Vector3 CatmullRomPosition(const std::vector<Vector3> &points, float t, bool isLoop = false);

    Vector3() noexcept = default;
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    explicit constexpr Vector3(float v) noexcept : x(v), y(v), z(v) {}
    Vector3(const Vector3 &vector) : x(vector.x), y(vector.y), z(vector.z) {}
    Vector3(const Vector2 &vector) noexcept;
    Vector3(const Vector4 &vector) noexcept;

    Vector3 &operator=(const Vector3 &vector) noexcept;
    Vector3 &operator+=(const Vector3 &vector) noexcept;
    Vector3 &operator-=(const Vector3 &vector) noexcept;
    Vector3 &operator*=(const float scalar) noexcept;
    Vector3 &operator*=(const Vector3 &vector) noexcept;
    Vector3 &operator/=(const float scalar);
    Vector3 &operator/=(const Vector3 &vector);
    bool operator==(const Vector3 &vector) const noexcept;
    bool operator!=(const Vector3 &vector) const noexcept;

    [[nodiscard]] constexpr float Dot(const Vector3 &vector) const noexcept;
    [[nodiscard]] Vector3 Cross(const Vector3 &vector) const noexcept;
    [[nodiscard]] float Length() const noexcept;
    [[nodiscard]] constexpr float LengthSquared() const noexcept;
    [[nodiscard]] Vector3 Normalize() const;
    [[nodiscard]] Vector3 Projection(const Vector3 &vector) const noexcept;
    [[nodiscard]] Vector3 ClosestPoint(const Math::Segment &segment) const noexcept;
    [[nodiscard]] Vector3 Perpendicular() const noexcept;
    [[nodiscard]] Vector3 Rejection(const Vector3 &vector) const noexcept;
    [[nodiscard]] Vector3 Refrection(const Vector3 &normal) const noexcept;
    [[nodiscard]] float Distance(const Vector3 &vector) const;
    [[nodiscard]] Vector3 Transform(const Matrix4x4 &mat) const noexcept;

    float x;
    float y;
    float z;
};


inline constexpr const Vector3 operator-(const Vector3 &vector) noexcept {
    return Vector3(-vector.x, -vector.y, -vector.z);
}

inline constexpr const Vector3 operator+(const Vector3 &vector1, const Vector3 &vector2) noexcept {
    return Vector3(vector1.x + vector2.x, vector1.y + vector2.y, vector1.z + vector2.z);
}

inline constexpr const Vector3 operator-(const Vector3 &vector1, const Vector3 &vector2) noexcept {
    return Vector3(vector1.x - vector2.x, vector1.y - vector2.y, vector1.z - vector2.z);
}

inline constexpr const Vector3 operator*(const Vector3 &vector, const float scalar) noexcept {
    return Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

inline constexpr const Vector3 operator*(const float scalar, const Vector3 &vector) noexcept {
    return Vector3(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}

inline constexpr const Vector3 operator/(const Vector3 &vector, const float scalar) {
    return vector * (1.0f / scalar);
}

inline constexpr const Vector3 operator/(const Vector3 &vector1, const Vector3 &vector2) {
    return Vector3(vector1.x / vector2.x, vector1.y / vector2.y, vector1.z / vector2.z);
}

const Vector3 operator*(const Matrix4x4 &mat, const Vector3 &vector) noexcept;
const Vector3 operator*(const Vector3 &vector, const Matrix4x4 &mat) noexcept;

} // namespace KashipanEngine