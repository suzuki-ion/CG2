#pragma once

namespace KashipanEngine {

struct Vector2;
struct Vector3;

struct Vector4 final {
    static Vector4 Lerp(const Vector4 &start, const Vector4 &end, float t) noexcept;

    Vector4() noexcept = default;
    constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    explicit constexpr Vector4(float value) : x(value), y(value), z(value), w(value) {}
    Vector4(const Vector2 &vector2) noexcept;
    Vector4(const Vector3 &vector3) noexcept;
    
    Vector4 &operator=(const Vector4 &vector) noexcept;
    Vector4 &operator+=(const Vector4 &vector) noexcept;
    Vector4 &operator-=(const Vector4 &vector) noexcept;
    Vector4 &operator*=(const float scalar) noexcept;
    Vector4 &operator*=(const Vector4 &vector) noexcept;
    Vector4 &operator/=(const float scalar);
    Vector4 &operator/=(const Vector4 &vector);
    bool operator==(const Vector4 &vector) const noexcept;
    bool operator!=(const Vector4 &vector) const noexcept;

    float x;
    float y;
    float z;
    float w;
};

inline constexpr const Vector4 operator-(const Vector4 &vector) noexcept {
    return Vector4(-vector.x, -vector.y, -vector.z, -vector.w);
}

inline constexpr const Vector4 operator+(const Vector4 &vector1, const Vector4 &vector2) noexcept {
    return Vector4(vector1.x + vector2.x, vector1.y + vector2.y, vector1.z + vector2.z, vector1.w + vector2.w);
}

inline constexpr const Vector4 operator-(const Vector4 &vector1, const Vector4 &vector2) noexcept {
    return Vector4(vector1.x - vector2.x, vector1.y - vector2.y, vector1.z - vector2.z, vector1.w - vector2.w);
}

inline constexpr const Vector4 operator*(const Vector4 &vector, const float scalar) noexcept {
    return Vector4(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
}

inline constexpr const Vector4 operator*(const float scalar, const Vector4 &vector) noexcept {
    return Vector4(scalar * vector.x, scalar * vector.y, scalar * vector.z, scalar * vector.w);
}

inline constexpr const Vector4 operator/(const Vector4 &vector, const float scalar) {
    return vector * (1.0f / scalar);
}

inline constexpr const Vector4 operator/(const Vector4 &vector1, const Vector4 &vector2) {
    return Vector4(vector1.x / vector2.x, vector1.y / vector2.y, vector1.z / vector2.z, vector1.w / vector2.w);
}

using float4 = Vector4;
using float32_t4 = Vector4;

} // namespace KashipanEngine