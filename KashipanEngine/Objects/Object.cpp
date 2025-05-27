#include "Object.h"

namespace KashipanEngine {

Object::Object(DirectXCommon *dxCommon) : dxCommon_(dxCommon) {
    // マテリアルリソースのマップを取得
    materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialMap));
    // TransformationMatrixリソースのマップを取得
    transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap));
    // UVTransformの初期化
    material.uvTransform.MakeIdentity();
    uvTransform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
}
} // namespace KashipanEngine