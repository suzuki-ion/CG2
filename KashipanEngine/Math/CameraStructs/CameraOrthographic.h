#pragma once

namespace KashipanEngine {

struct CameraOrthographic {
    float left = -1.0f;
    float top = 1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float nearClip = 0.1f;
    float farClip = 64.0f;
};

} // namespace KashipanEngine