#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace KashipanEngine {

struct DiffuseLight {
    /// @brief ライトの色
    Vector4 color;
    /// @brief ライトの向き
    Vector3 direction;
    /// @brief 輝度
    float intensity;
};

} // namespace KashipanEngine