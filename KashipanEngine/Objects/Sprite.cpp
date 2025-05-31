#include "Sprite.h"

namespace KashipanEngine {

Sprite::Sprite() {
    Create(4, 6);
    isUseCamera_ = false;
    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;
}

void Sprite::Draw() {
    // 法線を設定
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

} // namespace KashipanEngine