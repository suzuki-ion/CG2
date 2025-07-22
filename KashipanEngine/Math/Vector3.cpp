#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MathObjects/Lines.h"
#include <cassert>
#include <algorithm>

namespace KashipanEngine {

Vector3 Vector3::Lerp(const Vector3 &start, const Vector3 &end, float t) noexcept {
    return t * start + (1.0f - t) * end;
}

Vector3 Vector3::Slerp(const Vector3 &start, const Vector3 &end, float t) noexcept {
    Vector3 normalizedStart = start.Normalize();
    Vector3 normalizedEnd = end.Normalize();

    float dotProduct = normalizedStart.Dot(normalizedEnd);
    // Dotの値が変な値にならないよう制限
    dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);
    float angle = std::acos(dotProduct);
    float sinTheta = std::sin(angle);
    // 角度が0の場合は線形補間を行う
    if (sinTheta == 0.0f) {
        return Lerp(start, end, t).Normalize();
    }

    float t1 = std::sin(angle * (1.0f - t));
    float t2 = std::sin(angle * t);

    Vector3 result = (normalizedStart * t1 + normalizedEnd * t2) / sinTheta;
    return result.Normalize();
}

Vector3 Vector3::Bezier(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, float t) noexcept {
    Vector3 p01 = Vector3::Lerp(p0, p1, t);
    Vector3 p12 = Vector3::Lerp(p1, p2, t);
    return Vector3::Lerp(p01, p12, t);
}

Vector3 Vector3::CatmullRomInterpolation(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, float t) noexcept {
    const float s = 0.5f;

    float t2 = t * t;
    float t3 = t2 * t;

    Vector3 e3 = (-p0 + (3.0f * p1) - (3.0f * p2) + p3) * t3;
    Vector3 e2 = ((2.0f * p0) - (5.0f * p1) + (4.0f * p2) - p3) * t2;
    Vector3 e1 = (-p0 + p2) * t;
    Vector3 e0 = 2.0f * p1;

    return s * (e3 + e2 + e1 + e0);
}

Vector3 Vector3::CatmullRomPosition(const std::vector<Vector3> &points, float t) {
    assert(points.size() >= 4);

    // 区間数は制御点の数-1
    size_t division = points.size() - 1;
    // 1区間の長さ (全体を1.0とした割合)
    float areaWidth = 1.0f / static_cast<float>(division);

    // 区間内の始点を0.0f、終点を1.0fとしたときの現在位置
    float t2 = std::fmod(t, areaWidth) * static_cast<float>(division);
    // 下限(0.0f)と上限(1.0f)の範囲に収める
    t2 = std::clamp(t2, 0.0f, 1.0f);

    // 区間番号
    size_t index = static_cast<size_t>(t / areaWidth);
    // 区間番号が上限を超えないための計算
    index = std::min(index, division - 1);

    // 4点分のインデックス
    size_t index0 = index - 1;
    size_t index1 = index;
    size_t index2 = index + 1;
    size_t index3 = index + 2;

    // 最初の区間のp0はp1を重複使用する
    if (index == 0) {
        index0 = index1;
    }

    // 最後の区間のp3はp2を重複使用する
    if (index3 >= points.size()) {
        index3 = index2;
    }

    // 4点の座標
    const Vector3 &p0 = points[index0];
    const Vector3 &p1 = points[index1];
    const Vector3 &p2 = points[index2];
    const Vector3 &p3 = points[index3];

    // 4点を指定してCatmull-Rom補間
    return CatmullRomInterpolation(p0, p1, p2, p3, t2);
}

Vector3::Vector3(const Vector2 &vector) noexcept {
    x = vector.x;
    y = vector.y;
    z = 0.0f;
}

Vector3::Vector3(const Vector4 &vector) noexcept {
    if (vector.w == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    } else {
        x = vector.x / vector.w;
        y = vector.y / vector.w;
        z = vector.z / vector.w;
    }
}

Vector3 &Vector3::operator=(const Vector3 &vector) noexcept {
    x = vector.x;
    y = vector.y;
    z = vector.z;
    return *this;
}

Vector3 &Vector3::operator+=(const Vector3 &vector) noexcept {
    x += vector.x;
    y += vector.y;
    z += vector.z;
    return *this;
}

Vector3 &Vector3::operator-=(const Vector3 &vector) noexcept {
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    return *this;
}

Vector3 &Vector3::operator*=(const float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 &Vector3::operator*=(const Vector3 &vector) noexcept {
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;
    return *this;
}

Vector3 &Vector3::operator/=(const float scalar) {
    if (scalar == 0.0f) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    } else {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }
    return *this;
}

Vector3 &Vector3::operator/=(const Vector3 &vector) {
    if (vector.x == 0.0f) {
        x = 0.0f;
    } else {
        x /= vector.x;
    }
    if (vector.y == 0.0f) {
        y = 0.0f;
    } else {
        y /= vector.y;
    }
    if (vector.z == 0.0f) {
        z = 0.0f;
    } else {
        z /= vector.z;
    }
    return *this;
}

bool Vector3::operator==(const Vector3 &vector) const noexcept {
    return x == vector.x && y == vector.y && z == vector.z;
}

bool Vector3::operator!=(const Vector3 &vector) const noexcept {
    return x != vector.x || y != vector.y || z != vector.z;
}

constexpr float Vector3::Dot(const Vector3 &vector) const noexcept {
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3 Vector3::Cross(const Vector3 &vector) const noexcept {
    return Vector3(
        y * vector.z - z * vector.y,
        z * vector.x - x * vector.z,
        x * vector.y - y * vector.x
    );
}

float Vector3::Length() const noexcept {
    return std::sqrt(LengthSquared());
}

constexpr float Vector3::LengthSquared() const noexcept {
    return Dot(*this);
}

Vector3 Vector3::Normalize() const {
    const float length = Length();
    if (length == 0.0f) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    return *this / length;
}

Vector3 Vector3::Projection(const Vector3 &vector) const noexcept {
    return (Dot(vector) / vector.Dot(vector)) * vector;
}

Vector3 Vector3::ClosestPoint(const Math::Segment &segment) const noexcept {
    return segment.origin + (*this - segment.origin).Projection(segment.diff);
}

Vector3 Vector3::Perpendicular() const noexcept {
    if (x != 0.0f || y != 0.0f) {
        return Vector3(-y, x, 0.0f);
    }
    return Vector3(0.0f, -z, y);
}

Vector3 Vector3::Rejection(const Vector3 &vector) const noexcept {
    return *this - Projection(vector);
}

Vector3 Vector3::Refrection(const Vector3 &normal) const noexcept {
    return *this - 2.0f * Dot(normal) * normal;
}

float Vector3::Distance(const Vector3 &vector) const {
    return (vector - *this).Length();
}

Vector3 Vector3::Transform(const Matrix4x4 &mat) const noexcept {
    Vector3 result{};
    result.x = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + 1.0f * mat.m[3][0];
    result.y = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + 1.0f * mat.m[3][1];
    result.z = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + 1.0f * mat.m[3][2];
    float w = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + 1.0f * mat.m[3][3];

    if (w == 0.0f) {
        return Vector3(0.0f);
    }

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}

const Vector3 operator*(const Matrix4x4 &mat, const Vector3 &vector) noexcept {
    return Vector3(
        mat.m[0][0] * vector.x + mat.m[0][1] * vector.y + mat.m[0][2] * vector.z + mat.m[0][3],
        mat.m[1][0] * vector.x + mat.m[1][1] * vector.y + mat.m[1][2] * vector.z + mat.m[1][3],
        mat.m[2][0] * vector.x + mat.m[2][1] * vector.y + mat.m[2][2] * vector.z + mat.m[2][3]
    );
}

const Vector3 operator*(const Vector3 &vector, const Matrix4x4 &mat) noexcept {
    return Vector3(
        vector.x * mat.m[0][0] + vector.y * mat.m[1][0] + vector.z * mat.m[2][0] + mat.m[3][0],
        vector.x * mat.m[0][1] + vector.y * mat.m[1][1] + vector.z * mat.m[2][1] + mat.m[3][1],
        vector.x * mat.m[0][2] + vector.y * mat.m[1][2] + vector.z * mat.m[2][2] + mat.m[3][2]
    );
}

} // namespace KashipanEngine