#pragma once
#include "Math/Vector3.h"

namespace KashipanEngine {

struct Spring {
    // アンカー。固定された端の位置
    Vector3 anchor;
    // 自然長
    float naturalLength;
    // 剛性。バネ定数k
    float stiffness;
    // 減衰係数
    float dampingCoefficient;
};

} // namespace KashipanEngine