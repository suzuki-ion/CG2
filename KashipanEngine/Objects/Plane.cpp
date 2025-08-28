#include "Plane.h"

namespace KashipanEngine {

Plane::Plane() {
    Create(4, 6);
    isUseCamera_ = true;
    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;

    // 頂点座標
    mesh_->vertexBufferMap[0].position = { -0.5f, 0.0f, -0.5f, 1.0f };
    mesh_->vertexBufferMap[1].position = { -0.5f, 0.0f, 0.5f, 1.0f };
    mesh_->vertexBufferMap[2].position = { 0.5f, 0.0f, -0.5f, 1.0f };
    mesh_->vertexBufferMap[3].position = { 0.5f, 0.0f, 0.5f, 1.0f };
    // UV座標
    mesh_->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    mesh_->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    mesh_->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
}

void Plane::Draw() {
    // 法線を設定
    if (material_.lightingType == false) {
        for (int i = 0; i < 4; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 1.0f, 0.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3] = {};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

void Plane::Draw(WorldTransform &worldTransform) {
    // 法線を設定
    if (material_.lightingType == false) {
        for (int i = 0; i < 4; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 1.0f, 0.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3] = {};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画共通処理を呼び出す
    DrawCommon(worldTransform);
}

} // namespace KashipanEngine