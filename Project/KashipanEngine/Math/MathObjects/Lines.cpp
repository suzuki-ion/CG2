#include "Lines.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Math/Collider.h"

namespace KashipanEngine {

namespace Math {

//==================================================
// 直線
//==================================================
#pragma region Line

bool Line::IsCollision(const Plane &plane) const {
    return Collider::IsCollision(plane, *this);
}

bool Line::IsCollision(const Triangle &triangle) const {
    return Collider::IsCollision(triangle, *this);
}

bool Line::IsCollision(const AABB &aabb) const {
    return Collider::IsCollision(aabb, *this);
}

#pragma endregion

//==================================================
// 半直線
//==================================================
#pragma region Ray

bool Ray::IsCollision(const Plane &plane) const {
    return Collider::IsCollision(plane, *this);
}

bool Ray::IsCollision(const Triangle &triangle) const {
    return Collider::IsCollision(triangle, *this);
}

bool Ray::IsCollision(const AABB &aabb) const {
    return Collider::IsCollision(aabb, *this);
}

#pragma endregion

//==================================================
// 線分
//==================================================
#pragma region Segment

bool Segment::IsCollision(const Plane &plane) const {
    return Collider::IsCollision(plane, *this);
}

bool Segment::IsCollision(const Triangle &triangle) const {
    return Collider::IsCollision(triangle, *this);
}

bool Segment::IsCollision(const AABB &aabb) const {
    return Collider::IsCollision(aabb, *this);
}

#pragma endregion

} // namespace Math

} // namespace KashipanEngine