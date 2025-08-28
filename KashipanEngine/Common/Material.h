#pragma once
#include <cstdint>
#include "Math/Vector4.h"

namespace KashipanEngine {

struct Material {
    /// @brief マテリアルの色
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief Lightingの種類
    int32_t lightingType = 1;
    /// @brief padding分のメモリ
    float padding[3] = { 0.0f, 0.0f, 0.0f };
    /// @brief テクスチャ用のUVTransform行列
    Matrix4x4 uvTransform = Matrix4x4::Identity();
    /// @brief 拡散光の色
    Vector4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief 反射光の色
    Vector4 specularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief 放射光の色
    Vector4 emissiveColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

} // namespace KashipanEngine