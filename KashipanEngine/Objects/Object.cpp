#include <cassert>

#include "Object.h"
#include "Base/Renderer.h"
#include "Common/ConvertColor.h"
#include "Common/Logs.h"

namespace KashipanEngine {

Object::Object(Object &&other) noexcept {
    if (!other.mesh_) {
        return;
    }

    mesh_ = std::move(other.mesh_);
    materialResource_ = other.materialResource_;
    transformationMatrixResource_ = other.transformationMatrixResource_;
    materialMap_ = other.materialMap_;
    transformationMatrixMap_ = other.transformationMatrixMap_;
    vertexCount_ = other.vertexCount_;
    indexCount_ = other.indexCount_;
    useTextureIndex_ = other.useTextureIndex_;
}

void Object::DrawCommon() {
    // レンダラーが設定されていない場合はログを出力して終了
    if (renderer_ == nullptr) {
        Log("Renderer is not set.", kLogLevelFlagError);
        return;
    }

    // マテリアルを設定
    materialMap_->color = ConvertColor(material_.color);
    materialMap_->enableLighting = material_.enableLighting;
    materialMap_->uvTransform.MakeAffine(
        uvTransform_.scale,
        uvTransform_.rotate,
        uvTransform_.translate
    );

    // 行列を計算
    worldMatrix_.MakeAffine(
        transform_.scale,
        transform_.rotate,
        transform_.translate
    );
    // TransformationMatrixを転送
    transformationMatrixMap_->world = worldMatrix_;

    Renderer::ObjectState objectState;
    objectState.mesh = mesh_.get();
    objectState.materialResource = materialResource_.Get();
    objectState.transformationMatrixResource = transformationMatrixResource_.Get();
    objectState.transformationMatrixMap = transformationMatrixMap_;
    objectState.worldMatrix = &worldMatrix_;
    objectState.vertexCount = vertexCount_;
    objectState.indexCount = indexCount_;
    objectState.useTextureIndex = useTextureIndex_;
    objectState.fillMode = fillMode_;
    objectState.isUseCamera = isUseCamera_;
    bool isSemitransparent = (material_.color.w < 255.0f);
    renderer_->DrawSet(objectState, isUseCamera_, isSemitransparent);
}

void Object::DrawCommon(WorldTransform &worldTransform) {
    // レンダラーが設定されていない場合はログを出力して終了
    if (renderer_ == nullptr) {
        Log("Renderer is not set.", kLogLevelFlagError);
        return;
    }

    // マテリアルを設定
    materialMap_->color = ConvertColor(material_.color);
    materialMap_->enableLighting = material_.enableLighting;
    materialMap_->uvTransform.MakeAffine(
        uvTransform_.scale,
        uvTransform_.rotate,
        uvTransform_.translate
    );
    materialMap_->diffuseColor = ConvertColor(material_.diffuseColor);
    materialMap_->specularColor = ConvertColor(material_.specularColor);
    materialMap_->emissiveColor = ConvertColor(material_.emissiveColor);

    // ワールド行列を転送
    worldTransform.TransferMatrix();

    Renderer::ObjectState objectState;
    objectState.mesh = mesh_.get();
    objectState.materialResource = materialResource_.Get();
    objectState.transformationMatrixResource = worldTransform.GetTransformationMatrixResource();
    objectState.transformationMatrixMap = worldTransform.GetTransformationMatrixMap();
    objectState.worldMatrix = &worldTransform.worldMatrix_;
    objectState.vertexCount = vertexCount_;
    objectState.indexCount = indexCount_;
    objectState.useTextureIndex = useTextureIndex_;
    objectState.fillMode = fillMode_;
    objectState.isUseCamera = isUseCamera_;
    bool isSemitransparent = (material_.color.w < 255.0f);
    renderer_->DrawSet(objectState, isUseCamera_, isSemitransparent);
}

void Object::Create(UINT vertexCount, UINT indexCount) {
    // メッシュの生成
    mesh_ = PrimitiveDrawer::CreateMesh(vertexCount, indexCount);
    // 頂点数とインデックス数を設定
    vertexCount_ = vertexCount;
    indexCount_ = indexCount;

    // マテリアル用のリソースを生成
    materialResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(Material));
    // TransformationMatrix用のリソースを生成
    transformationMatrixResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));
    
    // マテリアルリソースのマップを取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void **>(&materialMap_));
    // TransformationMatrixリソースのマップを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap_));
    
    // UVTransformの初期化
    material_.uvTransform.MakeIdentity();
    uvTransform_ = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
}

} // namespace KashipanEngine