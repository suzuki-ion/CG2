#include "WorldTransform.h"
#include "3d/PrimitiveDrawer.h"

namespace KashipanEngine {

WorldTransform::WorldTransform() {
    // TransformationMatrixのリソースを作成
    transformationMatrixResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));
    // TransformationMatrixのマップを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap_));
}

void WorldTransform::TransferMatrix() {
    // ワールド変換行列を計算
    worldMatrix_.MakeAffine(scale_, rotate_, translate_);

    // 親のワールド変換がある場合は親のワールド行列とかける
    if (parentTransform_ != nullptr) {
        worldMatrix_ *= parentTransform_->worldMatrix_;
    }

    // TransformationMatrixの転送
    transformationMatrixMap_->world = worldMatrix_;
}

} // namespace KashipanEngine