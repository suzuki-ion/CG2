#include "Base/Texture.h"
#include "Cube.h"

KashipanEngine::Cube::Cube() {
    // 平面を6つ作成
    Create(4 * 6, 6 * 6);
    isUseCamera_ = true;

    for (UINT i = 0; i < 6; i++) {
        UINT index = i * 6;
        UINT vertexIndex = i * 4;
        mesh_->indexBufferMap[index + 0] = vertexIndex + 0;
        mesh_->indexBufferMap[index + 1] = vertexIndex + 1;
        mesh_->indexBufferMap[index + 2] = vertexIndex + 2;
        mesh_->indexBufferMap[index + 3] = vertexIndex + 1;
        mesh_->indexBufferMap[index + 4] = vertexIndex + 3;
        mesh_->indexBufferMap[index + 5] = vertexIndex + 2;
    }
    for (UINT i = 0; i < 6; i++) {
        UINT vertexIndex = i * 4;
        mesh_->vertexBufferMap[vertexIndex + 0].texCoord = { 0.0f, 1.0f };
        mesh_->vertexBufferMap[vertexIndex + 1].texCoord = { 0.0f, 0.0f };
        mesh_->vertexBufferMap[vertexIndex + 2].texCoord = { 1.0f, 1.0f };
        mesh_->vertexBufferMap[vertexIndex + 3].texCoord = { 1.0f, 0.0f };
    }
    
    UINT vertexIndex = 0;
    // 手前の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { -0.5f,  -0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { -0.5f,  0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { 0.5f, -0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { 0.5f, 0.5f, -0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { 0.0f, 0.0f, -1.0f };
    }
    vertexIndex += 4;
    // 奥の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { 0.5f,  -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { 0.5f,  0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { -0.5f, -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { -0.5f, 0.5f, 0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { 0.0f, 0.0f, 1.0f };
    }
    vertexIndex += 4;
    // 左の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { -0.5f,  -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { -0.5f,  0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { -0.5f, -0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { -0.5f, 0.5f, -0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { -1.0f, 0.0f, 0.0f };
    }
    vertexIndex += 4;
    // 右の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { 0.5f,  -0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { 0.5f,  0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { 0.5f, -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { 0.5f, 0.5f, 0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { 1.0f, 0.0f, 0.0f };
    }
    vertexIndex += 4;
    // 上の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { -0.5f,  0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { -0.5f,  0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { 0.5f, 0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { 0.5f, 0.5f, 0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { 0.0f, 1.0f, 0.0f };
    }
    vertexIndex += 4;
    // 下の面
    mesh_->vertexBufferMap[vertexIndex + 0].position = { -0.5f,  -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 1].position = { -0.5f,  -0.5f, -0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 2].position = { 0.5f, -0.5f, 0.5f , 1.0f };
    mesh_->vertexBufferMap[vertexIndex + 3].position = { 0.5f, -0.5f, -0.5f , 1.0f };
    for (UINT j = 0; j < 4; j++) {
        mesh_->vertexBufferMap[vertexIndex + j].normal = { 0.0f, -1.0f, 0.0f };
    }
}
