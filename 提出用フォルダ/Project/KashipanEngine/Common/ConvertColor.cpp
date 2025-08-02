#include "ConvertColor.h"
#include <algorithm>

namespace KashipanEngine {

Vector4 ConvertColor(unsigned int color) {
    return Vector4(
        static_cast<float>((color >> 24) & 0xFF) / 255.0f,
        static_cast<float>((color >> 16) & 0xFF) / 255.0f,
        static_cast<float>((color >> 8) & 0xFF) / 255.0f,
        static_cast<float>((color >> 0) & 0xFF) / 255.0f
    );
}

Vector4 ConvertColor(const Vector4 &color) {
    Vector4 convertColor;
    // 渡された値を0.0f～255.0fの間に収める
    convertColor.x = std::clamp(color.x, 0.0f, 255.0f);
    convertColor.y = std::clamp(color.y, 0.0f, 255.0f);
    convertColor.z = std::clamp(color.z, 0.0f, 255.0f);
    convertColor.w = std::clamp(color.w, 0.0f, 255.0f);
    // 0.0f～1.0fの間に収める
    convertColor.x /= 255.0f;
    convertColor.y /= 255.0f;
    convertColor.z /= 255.0f;
    convertColor.w /= 255.0f;
    return convertColor;
}

}