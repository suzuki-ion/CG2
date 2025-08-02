#pragma once
#include "Vector3.h"
#include <numbers>
#include <algorithm>

namespace KashipanEngine {

struct SphericalCoordinateSystem {
public:
    SphericalCoordinateSystem() = default;
    SphericalCoordinateSystem(const Vector3& vector, const Vector3& originPos) {
        origin = originPos;
        ToSpherical(vector);
    }

    /// @brief 3次元ベクトルを球面座標系に変換
    /// @param vector 変換する3次元ベクトル
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

    /// @brief 球面座標系を3次元ベクトルに変換
    /// @return 変換後の3次元ベクトル
    Vector3 ToVector3() const {
        return origin + Vector3(
            radius * sin(phi) * cos(theta),
            radius * cos(phi),
            radius * sin(phi) * sin(theta)
        );
    }

    /// @brief 球面座標系を3次元の回転ベクトルに変換
    /// @return 3次元の回転ベクトル
    Vector3 ToRotatedVector3() const {
        Vector3 forward = (origin - ToVector3()).Normalize();
        float yaw = atan2(forward.x, forward.z);
        float pitch = asinf(-forward.y /*std::clamp(-forward.y, -1.0f, 1.0f)*/);
        float roll = 0.0f;
        return Vector3(
            pitch,
            yaw,
            roll
        );
    }

    float radius = 1.0f;
    float theta = 0.0f;
    float phi = 0.0f;
    Vector3 origin = { 0.0f, 0.0f, 0.0f };
};

}