#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

namespace KashipanEngine {

namespace Math {

struct Line;
struct Ray;
struct Segment;

/// @brief 三角形
struct Triangle {
    Triangle() noexcept = default;
    Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c) noexcept
        : vertices{ a, b, c }
    {}

    /// @brief 三角形と直線の衝突判定
    /// @param line 衝突判定を行う直線
    /// @return 衝突しているかどうか
    [[nodiscard]] bool IsCollision(const Line &line) const;

    /// @brief 三角形と半直線の衝突判定
    /// @param ray 衝突判定を行う半直線
    /// @return 衝突しているかどうか
    [[nodiscard]] bool IsCollision(const Ray &ray) const;

    /// @brief 三角形と線分の衝突判定
    /// @param segment 衝突判定を行う線分
    /// @return 衝突しているかどうか
    [[nodiscard]] bool IsCollision(const Segment &segment) const;

    // 頂点
    Vector3 vertices[3];
};

} // namespace Math

} // namespace KashipanEngine