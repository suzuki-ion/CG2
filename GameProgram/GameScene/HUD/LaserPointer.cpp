#include "LaserPointer.h"
#include <Base/Texture.h>

using namespace KashipanEngine;

LaserPointer::LaserPointer() {
    worldTransform_ = std::make_unique<WorldTransform>();
    laserPointerCube_ = std::make_unique<Cube>();

    worldTransform_->scale_ = { 0.1f, 0.1f, 1.0f };

    auto state = laserPointerCube_->GetStatePtr();
    *state.useTextureIndex = Texture::Load("Resources/white1x1.png");
    state.material->lightingType = 0;
    state.material->color = { 0.0f, 1.0f, 0.0f, 0.25f };
    UINT vertexCount = laserPointerCube_->GetVertexCount();
    // 全頂点のZ座標を0.5f加算
    for (UINT i = 0; i < vertexCount; i++) {
        state.mesh->vertexBufferMap[i].position.z += 0.5f;
    }
}

void LaserPointer::Draw() {
    laserPointerCube_->Draw(*worldTransform_);
}