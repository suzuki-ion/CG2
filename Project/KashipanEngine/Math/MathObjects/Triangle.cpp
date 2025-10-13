#include "Triangle.h"
#include "Math/Collider.h"

namespace KashipanEngine {

namespace Math {

bool Triangle::IsCollision(const Line &line) const {
    return Collider::IsCollision(*this, line);
}

bool Triangle::IsCollision(const Ray &ray) const {
    return Collider::IsCollision(*this, ray);
}

bool Triangle::IsCollision(const Segment &segment) const {
    return Collider::IsCollision(*this, segment);
}

} // namespace Math

} // namespace KashipanEngine
