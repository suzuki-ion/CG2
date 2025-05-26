#include "Tetrahedron.h"
#include <cmath>

KashipanEngine::Tetrahedron::Tetrahedron() {
    mesh = PrimitiveDrawer::CreateMesh(12, 12);

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
    mesh->vertexBufferMap[0].position = vertices[0];
    mesh->vertexBufferMap[1].position = vertices[1];
    mesh->vertexBufferMap[2].position = vertices[2];
    
    mesh->vertexBufferMap[3].position = vertices[0];
    mesh->vertexBufferMap[4].position = vertices[3];
    mesh->vertexBufferMap[5].position = vertices[1];

    mesh->vertexBufferMap[6].position = vertices[1];
    mesh->vertexBufferMap[7].position = vertices[3];
    mesh->vertexBufferMap[8].position = vertices[2];

    mesh->vertexBufferMap[9].position = vertices[2];
    mesh->vertexBufferMap[10].position = vertices[3];
    mesh->vertexBufferMap[11].position = vertices[0];

    // それぞれのuv座標を設定
    for (int i = 0; i < 4; i++) {
        mesh->vertexBufferMap[i * 3 + 0].texCoord = { 0.0f, 1.0f };
        mesh->vertexBufferMap[i * 3 + 1].texCoord = { 0.5f, 0.0f };
        mesh->vertexBufferMap[i * 3 + 2].texCoord = { 1.0f, 1.0f };
    }

    // インデックスを設定
    for (int i = 0; i < 12; i++) {
        mesh->indexBufferMap[i] = i;
    }
}
