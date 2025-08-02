#pragma once
#include "Math/AffineMatrix.h"
#include "Common/TransformationMatrix.h"
#include <wrl.h>
#include <d3d12.h>

namespace KashipanEngine {

class WorldTransform {
public:
    // ワールド変換の初期化
    WorldTransform();
    // 行列の転送
    void TransferMatrix();
    
    Vector3 translate_ = { 0.0f, 0.0f, 0.0f };
    Vector3 rotate_ = { 0.0f, 0.0f, 0.0f };
    Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    // ワールド行列
    Matrix4x4 worldMatrix_;
    // 親のWorldTransformへのポインタ
    const WorldTransform *parentTransform_ = nullptr;

    /// @brief リソースへのポインタを取得
    /// @return TransformationMatrixリソースへのポインタ
    [[nodiscard]] ID3D12Resource *GetTransformationMatrixResource() const {
        return transformationMatrixResource_.Get();
    }

    /// @brief TransformationMatrixのマップを取得
    /// @return TransformationMatrixのマップへのポインタ
    [[nodiscard]] TransformationMatrix *GetTransformationMatrixMap() const {
        return transformationMatrixMap_;
    }
    
private:
    // コピー禁止。コピーされるとポインタが無効になるため。
    WorldTransform(const WorldTransform &) = delete;
    WorldTransform &operator=(const WorldTransform &) = delete;

    // TransformationMatrix用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    // TransformationMatrixマップ
    TransformationMatrix *transformationMatrixMap_ = nullptr;
};

static_assert(!std::is_copy_assignable_v<WorldTransform>);

} // namespace KashipanEngine