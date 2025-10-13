#include "ConicalPendulum.h"
#include "Math/MathObjects/Lines.h"
#include "Math/MathObjects/Sphere.h"
#include <cmath>

namespace KashipanEngine {

void ConicalPendulum::CalculateAngle(const float deltaTime) {
    // 重力加速度
    static const float g = 9.8f;

    angularVelocity = std::sqrt(g / (length * std::cos(halfApexAngle)));
    angle += angularVelocity * deltaTime;

    float radius = std::sin(halfApexAngle) * length;
    float height = std::cos(halfApexAngle) * length;

    // 振り子の先端位置を計算
    bob.x = anchor.x + std::cos(angle) * radius;
    bob.y = anchor.y - height;
    bob.z = anchor.z - std::sin(angle) * radius;
}

} // namespace KashipanEngine