#pragma once
#include "Math/Matrix4x4.h"

namespace KashipanEngine {

struct TransformationMatrix {
    /// @brief WVP行列
    Matrix4x4 wvp;
    /// @brief ワールド行列
    Matrix4x4 world;
};

} // namespace KashipanEngine