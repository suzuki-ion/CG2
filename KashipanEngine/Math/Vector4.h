#pragma once

namespace KashipanEngine {

struct Vector4 final {
    Vector4() = default;
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(float value) : x(value), y(value), z(value), w(value) {}
    
    float x;
    float y;
    float z;
    float w;
};

} // namespace KashipanEngine