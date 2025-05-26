#pragma once
#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "Common/TransformationMatrix.h"
#include "Common/Material.h"
#include "3d/PrimitiveDrawer.h"

namespace KashipanEngine {

// 前方宣言
class Camera;

enum NormalType {
    kNormalTypeVertex,
    kNormalTypeFace,
};

struct Object {
    Object() {
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
    virtual ~Object() {
        // マテリアルリソースのアンマップ
        if (materialMap != nullptr) {
            materialResource->Unmap(0, nullptr);
        }
        // TransformationMatrixリソースのアンマップ
        if (transformationMatrixMap != nullptr) {
            transformationMatrixResource->Unmap(0, nullptr);
        }
    }
    Object(Object &&other) noexcept {
        mesh = std::move(other.mesh);
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
        useTextureIndex = other.useTextureIndex;
        normalType = other.normalType;
        fillMode = other.fillMode;
    }

    /// @brief カメラへのポインタ
    Camera *camera = nullptr;

    /// @brief マテリアル用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> materialResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(Material));
    /// @brief TransformationMatrix用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));
    /// @brief メッシュ
    std::unique_ptr<Mesh> mesh;

    /// @brief マテリアルマップ
    Material *materialMap = nullptr;
    /// @brief TransformationMatrixマップ
    TransformationMatrix *transformationMatrixMap = nullptr;

    /// @brief 変形用のtransform
    Transform transform = {
        { 1.0f, 1.0f, 1.0f },   // スケール
        { 0.0f, 0.0f, 0.0f },   // 回転
        { 0.0f, 0.0f, 0.0f }    // 平行移動
    };
    /// @brief UV用のtransform
    Transform uvTransform = {
        { 1.0f, 1.0f, 1.0f },   // スケール
        { 0.0f, 0.0f, 0.0f },   // 回転
        { 0.0f, 0.0f, 0.0f }    // 平行移動
    };
    /// @brief ワールド行列
    Matrix4x4 worldMatrix;
    /// @brief マテリアルデータ
    Material material;

    /// @brief 使用するテクスチャのインデックス
    int useTextureIndex = -1;
    /// @brief 法線のタイプ
    NormalType normalType = kNormalTypeVertex;
    /// @brief 塗りつぶしモード
    FillMode fillMode = kFillModeSolid;
};

} // namespace KashipanEngine