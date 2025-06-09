#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

namespace KashipanEngine {

namespace Math {

struct Plane;

struct Sphere {
    Sphere() noexcept : center{ 0.0f, 0.0f, 0.0f }, radius(1.0f) {}
    Sphere(const Vector3 &center, float radius) noexcept :
        center(center), radius(radius)
    {}
    Sphere(const Sphere &sphere) noexcept :
        center(sphere.center), radius(sphere.radius)
    {}

    /// @brief 弾との衝突判定
    /// @param sphere 衝突判定を行う球
    /// @return 衝突しているかどうか
    [[nodiscard]] bool IsCollision(const Sphere &sphere) const;

    /// @brief 平面との衝突判定
    /// @param plane 衝突判定を行う平面
    /// @return 衝突しているかどうか
    [[nodiscard]] bool IsCollision(const Plane &plane) const;

    Vector3 center;
    float radius;
};

} // namespace Math

} // namespace KashipanEngine