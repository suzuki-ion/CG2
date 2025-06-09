#include "AABB.h"
#include "Math/Collider.h"
#include <algorithm>

namespace KashipanEngine {

namespace Math {

AABB::AABB(const Vector3 &min, const Vector3 &max) noexcept {
    this->min = min;
    this->max = max;
}

AABB::AABB(const AABB &aabb) noexcept {
    this->min = aabb.min;
    this->max = aabb.max;
}

void AABB::Sort() noexcept {
    if (min.x > max.x) {
        std::swap(min.x, max.x);
    }
    if (min.y > max.y) {
        std::swap(min.y, max.y);
    }
    if (min.z > max.z) {
        std::swap(min.z, max.z);
    }
}

bool AABB::IsCollision(const AABB &aabb) const {
    return Collider::IsCollision(*this, aabb);
}

bool AABB::IsCollision(const Sphere &sphere) const {
    return Collider::IsCollision(*this, sphere);
}

bool AABB::IsCollision(const Line &line) const {
    return Collider::IsCollision(*this, line);
}

bool AABB::IsCollision(const Ray &ray) const {
    return Collider::IsCollision(*this, ray);
}

bool AABB::IsCollision(const Segment &segment) const {
    return Collider::IsCollision(*this, segment);
}

} // namespace Math

} // namespace KashipanEngine