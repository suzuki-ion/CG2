#include "Bezier.h"

namespace KashipanEngine {

Vector3 Bezier(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const float t) noexcept {
    Vector3 p01 = Vector3::Lerp(p0, p1, t);
    Vector3 p12 = Vector3::Lerp(p1, p2, t);
    return Vector3::Lerp(p01, p12, t);
}

} // namespace KashipanEngine