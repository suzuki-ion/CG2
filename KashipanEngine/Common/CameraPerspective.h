#pragma once

namespace KashipanEngine {

struct CameraPerspective {
    float fovY = 0.45f;
    float aspectRatio = 1280.0f / 720.0f;
    float nearClip = 0.1f;
    float farClip = 2048.0f;
};

} // namespace KashipanEngine