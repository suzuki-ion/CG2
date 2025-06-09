#include "Sprite.h"
#include "Base/Texture.h"
#include "Common/Logs.h"

namespace KashipanEngine {

Sprite::Sprite(const std::string &filePath) {
    Create(4, 6);
    isUseCamera_ = false;
    material_.enableLighting = false;
    useTextureIndex_ = Texture::Load(filePath);
    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;

    auto &textureData = Texture::GetTexture(useTextureIndex_);
    float width = static_cast<float>(textureData.width);
    float height = static_cast<float>(textureData.height);
    mesh_->vertexBufferMap[0].position = { 0.0f,    height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].position = { 0.0f,    0.0f,   0.0f, 1.0f };
    mesh_->vertexBufferMap[2].position = { width,   height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[3].position = { width,   0.0f,   0.0f, 1.0f };

    mesh_->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    mesh_->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    mesh_->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
}

Sprite::Sprite(const uint32_t textureIndex) {
    Create(4, 6);
    isUseCamera_ = false;
    material_.enableLighting = false;
    useTextureIndex_ = textureIndex;
    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;

    auto &textureData = Texture::GetTexture(useTextureIndex_);
    float width = static_cast<float>(textureData.width);
    float height = static_cast<float>(textureData.height);
    mesh_->vertexBufferMap[0].position = { 0.0f,    height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].position = { 0.0f,    0.0f,   0.0f, 1.0f };
    mesh_->vertexBufferMap[2].position = { width,   height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[3].position = { width,   0.0f,   0.0f, 1.0f };

    mesh_->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    mesh_->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    mesh_->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
}

void Sprite::Draw() {
    // 法線を設定
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

void Sprite::Draw(WorldTransform &worldTransform) {
    // 法線を設定
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
    }
    // 描画共通処理を呼び出す
    DrawCommon(worldTransform);
}

} // namespace KashipanEngine