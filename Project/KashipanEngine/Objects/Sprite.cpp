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

void Sprite::SetPivot(const Vector2 &pivot) {
    if (pivot_ == pivot) {
        return;
    }
    if (pivot_.x != pivot.x) {
        AdjustVertexPosX(pivot.x);
    }
    if (pivot_.y != pivot.y) {
        AdjustVertexPosY(pivot.y);
    }
    pivot_ = pivot;
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

void Sprite::AdjustVertexPosX(float newPivotX) {
    // 座標を一度原点に戻す
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].position.x += width_ * pivot_.x;
    }
    // 新しいピボットで座標を調整
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].position.x -= width_ * newPivotX;
    }
}

void Sprite::AdjustVertexPosY(float newPivotY) {
    // 座標を一度原点に戻す
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].position.y += height_ * pivot_.y;
    }
    // 新しいピボットで座標を調整
    for (int i = 0; i < 4; i++) {
        mesh_->vertexBufferMap[i].position.y -= height_ * newPivotY;
    }
}

void Sprite::LoadTexture(const TextureData &textureData) {
    width_ = static_cast<float>(textureData.width);
    height_ = static_cast<float>(textureData.height);

    mesh_->vertexBufferMap[0].position = { 0.0f,    height_,    0.0f, 1.0f };
    mesh_->vertexBufferMap[1].position = { 0.0f,    0.0f,       0.0f, 1.0f };
    mesh_->vertexBufferMap[2].position = { width_,  height_,    0.0f, 1.0f };
    mesh_->vertexBufferMap[3].position = { width_,  0.0f,       0.0f, 1.0f };

    // ピボットの位置で頂点を調整
    Vector2 currentPivot = pivot_;
    pivot_ = { 0.0f, 0.0f };
    SetPivot(currentPivot);
}

} // namespace KashipanEngine