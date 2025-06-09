#include "Tetrahedron.h"
#include <cmath>

namespace KashipanEngine {

Tetrahedron::Tetrahedron() {
    Create(12, 12);
    isUseCamera_ = true;

    // 正四面体の頂点座標
    Vector4 vertices[4] = {
        // 底面
        {  0.5f,  0.5f,  0.5f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f }
    };

    //==================================================
    // 頂点座標の設定
    //==================================================

    // 底面
    mesh_->vertexBufferMap[0].position = vertices[0];
    mesh_->vertexBufferMap[1].position = vertices[1];
    mesh_->vertexBufferMap[2].position = vertices[2];
    
    mesh_->vertexBufferMap[3].position = vertices[0];
    mesh_->vertexBufferMap[4].position = vertices[3];
    mesh_->vertexBufferMap[5].position = vertices[1];

    mesh_->vertexBufferMap[6].position = vertices[1];
    mesh_->vertexBufferMap[7].position = vertices[3];
    mesh_->vertexBufferMap[8].position = vertices[2];

    mesh_->vertexBufferMap[9].position = vertices[2];
    mesh_->vertexBufferMap[10].position = vertices[3];
    mesh_->vertexBufferMap[11].position = vertices[0];

    // それぞれのuv座標を設定
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i * 3 + 0].texCoord = { 0.0f, 1.0f };
        mesh_->vertexBufferMap[i * 3 + 1].texCoord = { 0.5f, 0.0f };
        mesh_->vertexBufferMap[i * 3 + 2].texCoord = { 1.0f, 1.0f };
    }

    // インデックスを設定
    for (int i = 0; i < 12; i++) {
        mesh_->indexBufferMap[i] = i;
    }
}

void Tetrahedron::Draw() {
    // 法線を設定
    if (material_.enableLighting == false) {
        for (int i = 0; i < 12; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 12; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3]{};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 12; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

void Tetrahedron::Draw(WorldTransform &worldTransform) {
    // 法線を設定
    if (material_.enableLighting == false) {
        for (int i = 0; i < 12; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 12; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3] = {};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 12; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // 描画共通処理を呼び出す
    DrawCommon(worldTransform);
}

} // namespace KashipanEngine