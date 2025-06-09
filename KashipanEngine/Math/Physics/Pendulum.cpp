#include "Pendulum.h"
#include "Math/MathObjects/Lines.h"
#include "Math/MathObjects/Sphere.h"
#include <cmath>

namespace KashipanEngine {

void Pendulum::CalculateAngle(const float deltaTime) {
    // 重力加速度
    static const float g = 9.8f;

    angularAcceleration = (-g / length) * std::sin(angle);
    angularVelocity += angularAcceleration * deltaTime;
    angle += angularVelocity * deltaTime;

    bob.x = anchor.x + std::sin(angle) * length;
    bob.y = anchor.y - std::cos(angle) * length;
    bob.z = anchor.z;
}

} // namespace KashipanEngine