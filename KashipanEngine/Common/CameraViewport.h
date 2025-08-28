#pragma once

namespace KashipanEngine {

struct CameraViewport {
    float left = 0.0f;
    float top = 0.0f;
    float width = 1280.0f;
    float height = 720.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

} // namespace KashipanEngine