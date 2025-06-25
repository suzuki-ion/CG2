#pragma once
#include "Vector3.h"

namespace KashipanEngine {

struct SphericalCoordinateSystem {
public:
    SphericalCoordinateSystem() = default;
    SphericalCoordinateSystem(const Vector3& vector, const Vector3& originPos) {
        origin = originPos;
        ToSpherical(vector);
    }

    void ToSpherical(const Vector3& vector) {
        radius = vector.Length();
        if (radius > 0.0f) {
            theta = atan2(vector.z, vector.x);
            phi = acos(vector.y / radius);
        } else {
            theta = 0.0f;
            phi = 0.0f;
        }
    }

    Vector3 ToVector3() const {
        return origin + Vector3(
            radius * sin(phi) * cos(theta),
            radius * cos(phi),
            radius * sin(phi) * sin(theta)
        );
    }

    float radius = 1.0f;
    float theta = 0.0f;
    float phi = 0.0f;
    Vector3 origin = { 0.0f, 0.0f, 0.0f };
};

}