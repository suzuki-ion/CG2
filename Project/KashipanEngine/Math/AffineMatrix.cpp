#include "AffineMatrix.h"

namespace KashipanEngine {

AffineMatrix::AffineMatrix(const Matrix4x4 &scale, const Matrix4x4 &rotate, const Matrix4x4 &translate) noexcept {
    scaleMatrix = scale;
    rotateMatrix = rotate;
    translateMatrix = translate;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

AffineMatrix::AffineMatrix(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate) noexcept {
    scaleMatrix.MakeScale(scale);
    rotateMatrix.MakeRotate(rotate);
    translateMatrix.MakeTranslate(translate);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

AffineMatrix::AffineMatrix(const AffineMatrix &affine) noexcept {
    scaleMatrix = affine.GetScaleMatrix();
    rotateMatrix = affine.rotateMatrix;
    translateMatrix = affine.translateMatrix;
    worldMatrix = affine.worldMatrix;
}

AffineMatrix &AffineMatrix::operator=(const AffineMatrix &affine) noexcept {
    scaleMatrix = affine.scaleMatrix;
    rotateMatrix = affine.rotateMatrix;
    translateMatrix = affine.translateMatrix;
    worldMatrix = affine.worldMatrix;
    return *this;
}

AffineMatrix &AffineMatrix::operator*=(const AffineMatrix &affine) noexcept {
    scaleMatrix *= affine.scaleMatrix;
    rotateMatrix *= affine.rotateMatrix;
    translateMatrix *= affine.translateMatrix;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    return *this;
}

AffineMatrix &AffineMatrix::operator*=(const Matrix4x4 &mat) noexcept {
    scaleMatrix *= mat;
    rotateMatrix *= mat;
    translateMatrix *= mat;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    return *this;
}

void AffineMatrix::SetSRT(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate) {
    scaleMatrix.MakeScale(scale);
    rotateMatrix.MakeRotate(rotate);
    translateMatrix.MakeTranslate(translate);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetScale(const Vector3 &scale) noexcept {
    scaleMatrix.MakeScale(scale);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetScale(const Matrix4x4 &scale) noexcept {
    scaleMatrix = scale;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetScale(float scaleX, float scaleY, float scaleZ) noexcept {
    scaleMatrix.MakeScale({ scaleX, scaleY, scaleZ });
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetScale(float scale) noexcept {
    scaleMatrix.MakeScale({ scale, scale, scale });
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetRotate(const Vector3 &rotate) noexcept {
    rotateMatrix.MakeRotate(rotate);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetRotate(const Matrix4x4 &rotate) noexcept {
    rotateMatrix = rotate;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetRotate(float radianX, float radianY, float radianZ) noexcept {
    rotateMatrix.MakeRotate(radianX, radianY, radianZ);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetTranslate(const Vector3 &translate) noexcept {
    translateMatrix.MakeTranslate(translate);
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetTranslate(const Matrix4x4 &translate) noexcept {
    translateMatrix = translate;
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

void AffineMatrix::SetTranslate(float translateX, float translateY, float translateZ) noexcept {
    translateMatrix.MakeTranslate({ translateX, translateY, translateZ });
    worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
}

Matrix4x4 AffineMatrix::InverseScale() const noexcept {
    return Matrix4x4(
        1.0f / scaleMatrix.m[0][0], 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / scaleMatrix.m[1][1], 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / scaleMatrix.m[2][2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

Matrix4x4 AffineMatrix::InverseRotate() const noexcept {
    return Matrix4x4(
        rotateMatrix.m[0][0], rotateMatrix.m[1][0], rotateMatrix.m[2][0], 0.0f,
        rotateMatrix.m[0][1], rotateMatrix.m[1][1], rotateMatrix.m[2][1], 0.0f,
        rotateMatrix.m[0][2], rotateMatrix.m[1][2], rotateMatrix.m[2][2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

Matrix4x4 AffineMatrix::InverseTranslate() const noexcept {
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -translateMatrix.m[3][0], -translateMatrix.m[3][1], -translateMatrix.m[3][2], 1.0f
    );
}

} // namespace KashipanEngine