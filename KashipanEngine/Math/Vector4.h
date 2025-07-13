#pragma once

namespace KashipanEngine {

struct Vector2;
struct Vector3;

struct Vector4 final {
    Vector4() = default;
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(float value) : x(value), y(value), z(value), w(value) {}
    Vector4(const Vector2 &vector2);
    Vector4(const Vector3 &vector3);
    
    float x;
    float y;
    float z;
    float w;
};

} // namespace KashipanEngine