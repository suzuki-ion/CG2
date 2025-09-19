#pragma once

namespace KashipanEngine {

namespace Math {

struct Sphere;
struct Line;
struct Ray;
struct Segment;
struct Plane;
struct Triangle;
struct AABB;

namespace Collider {

/// @brief 球と球の衝突判定
/// @param sphere1 衝突判定を行う球1
/// @param sphere2 衝突判定を行う球2
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Sphere &sphere1, const Sphere &sphere2);

/// @brief 球と平面の衝突判定
/// @param sphere 衝突判定を行う球
/// @param plane 衝突判定を行う平面
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Sphere &sphere, const Plane &plane);

/// @brief 平面と直線の衝突判定
/// @param plane 衝突判定を行う平面
/// @param line 衝突判定を行う直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Plane &plane, const Line &line);

/// @brief 平面と半直線の衝突判定
/// @param plane 衝突判定を行う平面
/// @param ray 衝突判定を行う半直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Plane &plane, const Ray &ray);

/// @brief 平面と線分の衝突判定
/// @param plane 衝突判定を行う平面
/// @param segment 衝突判定を行う線分
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Plane &plane, const Segment &segment);

/// @brief 三角形と直線の衝突判定
/// @param triangle 衝突判定を行う三角形
/// @param line 衝突判定を行う直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Triangle &triangle, const Line &line);

/// @brief 三角形と半直線の衝突判定
/// @param triangle 衝突判定を行う三角形
/// @param ray 衝突判定を行う半直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Triangle &triangle, const Ray &ray);

/// @brief 三角形と線分の衝突判定
/// @param triangle 衝突判定を行う三角形
/// @param segment 衝突判定を行う線分
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const Triangle &triangle, const Segment &segment);

/// @brief AABBとAABBの衝突判定
/// @param aabb1 衝突判定を行うAABB1
/// @param aabb2 衝突判定を行うAABB2
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const AABB &aabb1, const AABB &aabb2);

/// @brief AABBと球の衝突判定
/// @param aabb 衝突判定を行うAABB
/// @param sphere 衝突判定を行う球
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const AABB &aabb, const Sphere &sphere);

/// @brief AABBと直線の衝突判定
/// @param aabb 衝突判定を行うAABB
/// @param line 衝突判定を行う直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const AABB &aabb, const Line &line);

/// @brief AABBと半直線の衝突判定
/// @param aabb 衝突判定を行うAABB
/// @param ray 衝突判定を行う半直線
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const AABB &aabb, const Ray &ray);

/// @brief AABBと線分の衝突判定
/// @param aabb 衝突判定を行うAABB
/// @param segment 衝突判定を行う線分
/// @return 衝突しているかどうか
[[nodiscard]] bool IsCollision(const AABB &aabb, const Segment &segment);

} // namespace Collider

} // namespace Math

} // namespace KashipanEngine