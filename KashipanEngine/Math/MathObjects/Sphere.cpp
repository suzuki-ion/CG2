#include "Sphere.h"
#include "Math/Collider.h"
#include <cmath>
#define M_PI (4.0f * std::atanf(1.0f))

namespace KashipanEngine {

namespace Math {

bool Sphere::IsCollision(const Sphere &sphere) const {
    return Collider::IsCollision(*this, sphere);
}

bool Sphere::IsCollision(const Plane &plane) const {
    return Collider::IsCollision(*this, plane);
}

} // namespace Math

} // namespace KashipanEngine