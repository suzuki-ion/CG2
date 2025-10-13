#pragma once
#include "Math/Vector3.h"

namespace KashipanEngine {

struct ConicalPendulum {
    /// @brief 円錐振り子の角度を計算する
    /// @param deltaTime 前回の更新からの経過時間
    void CalculateAngle(const float deltaTime);

    // 支点の位置
    Vector3 anchor;
    // 振り子の先端位置
    Vector3 bob;
    // 振り子の長さ
    float length;
    // 円錐の頂角の半分
    float halfApexAngle;
    // 振り子の角度
    float angle;
    // 振り子の角速度
    float angularVelocity;
};

} // namespace KashipanEngine