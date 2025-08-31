#include "Sprite.h"
#include "Base/Texture.h"
#include "Common/Logs.h"

namespace KashipanEngine {

Sprite::Sprite(const std::string &filePath) {
    Initialize();
    SetTexture(filePath);
}

Sprite::Sprite(const uint32_t textureIndex) {
    Initialize();
    SetTexture(textureIndex);
}

void Sprite::SetTexture(const std::string &filePath) {
    useTextureIndex_ = Texture::Load(filePath);
    LoadTexture(Texture::GetTexture(filePath));
}

void Sprite::SetTexture(const uint32_t textureIndex) {
    useTextureIndex_ = textureIndex;
    LoadTexture(Texture::GetTexture(useTextureIndex_));
}

void Sprite::Initialize() {
    name_ = "Sprite";

    Create(4, 6);
    isUseCamera_ = false;
    material_.lightingType = 0;

    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;

    mesh_->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    mesh_->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    mesh_->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };

    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
    }
}

void Sprite::LoadTexture(const TextureData &textureData) {
    float width = static_cast<float>(textureData.width);
    float height = static_cast<float>(textureData.height);

    mesh_->vertexBufferMap[0].position = { 0.0f,    height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[1].position = { 0.0f,    0.0f,   0.0f, 1.0f };
    mesh_->vertexBufferMap[2].position = { width,   height, 0.0f, 1.0f };
    mesh_->vertexBufferMap[3].position = { width,   0.0f,   0.0f, 1.0f };
}

} // namespace KashipanEngine