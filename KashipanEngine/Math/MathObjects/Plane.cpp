#include "Plane.h"
#include "Math/Collider.h"
#include <cmath>

namespace KashipanEngine {

namespace Math {

void Plane::Set(const Vector3 &n, const Vector3 &p) noexcept {
    normal = n;
    // 平面の距離を求める
    distance = n.Dot(p);
}

void Plane::Set(const Vector3 &a, const Vector3 &b, const Vector3 &c) noexcept {
    // 3点から法線ベクトルを求める
    normal = (b - a).Cross(c - b).Normalize();
    // 平面の距離を求める
    distance = normal.Dot(a);
}

bool Plane::IsCollision(const Sphere &sphere) const {
    return Collider::IsCollision(sphere, *this);
}

bool Plane::IsCollision(const Line &line) const {
    return Collider::IsCollision(*this, line);
}

bool Plane::IsCollision(const Ray &ray) const {
    return Collider::IsCollision(*this, ray);
}

bool Plane::IsCollision(const Segment &segment) const {
    return Collider::IsCollision(*this, segment);
}

} // namespace Math

} // namespace KashipanEngine