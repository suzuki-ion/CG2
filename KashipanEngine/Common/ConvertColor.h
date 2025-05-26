#pragma once
#include "Math/Vector4.h"

namespace KashipanEngine {

/// @brief 32bitの色を正規化されたRGBAに変換
/// @param color 32bitの色
/// @return 正規化されたRGBAの色
Vector4 ConvertColor(unsigned int color);

/// @brief 32bitの色を正規化されたRGBAに変換
/// @param color 32bitの色
/// @return 正規化されたRGBAの色
Vector4 ConvertColor(const Vector4 &color);

};