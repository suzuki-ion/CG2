#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace KashipanEngine {

Vector4::Vector4(const Vector2 &vector2) {
    x = vector2.x;
    y = vector2.y;
    z = 0.0f;
    w = 1.0f;
}

Vector4::Vector4(const Vector3 &vector3) {
    x = vector3.x;
    y = vector3.y;
    z = vector3.z;
    w = 1.0f;
}

} // namespace KashipanEngine