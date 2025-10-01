#include "PlayerShootCoolDownBar.h"

using namespace KashipanEngine;

PlayerShootCoolDownBar::PlayerShootCoolDownBar(Camera *camera) {
    camera_ = camera;
    worldTransform_ = std::make_unique<WorldTransform>();
    backGroundSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    coolDownBarSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");

    backGroundSprite_->GetStatePtr().material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    coolDownBarSprite_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 1.0f, 1.0f);
    
    CalculateBackGroundSpriteVertexPos();
    CalculateCoolDownBarSpriteVertexPos();
}

void PlayerShootCoolDownBar::SetPosition(const Vector3 &position) {
    Vector3 screenPos = position.Transform(camera_->GetViewMatrix() * camera_->GetProjectionMatrix());
    screenPos.x = (screenPos.x + 1.0f) * 0.5f * 1920.0f;
    screenPos.y = (1.0f - screenPos.y) * 0.5f * 1080.0f + 32.0f;
    worldTransform_->translate_ = screenPos;
}

void PlayerShootCoolDownBar::SetCoolDownMax(float coolDownMax) {
    coolDownMax_ = coolDownMax;
    CalculateCoolDownBarSpriteVertexPos();
}

void PlayerShootCoolDownBar::SetCoolDownCurrent(float coolDownCurrent) {
    coolDownCurrent_ = coolDownCurrent;
    if (coolDownCurrent_ < 0.0f) {
        coolDownCurrent_ = 0.0f;
    } else if (coolDownCurrent_ > coolDownMax_) {
        coolDownCurrent_ = coolDownMax_;
    }
    CalculateCoolDownBarSpriteVertexPos();
}

void PlayerShootCoolDownBar::Draw() {
    if (coolDownCurrent_ <= 0.0f) {
        return;
    }
    backGroundSprite_->Draw(*worldTransform_);
    coolDownBarSprite_->Draw(*worldTransform_);
}

void PlayerShootCoolDownBar::CalculateBackGroundSpriteVertexPos() {
    float halfWidth = width_ / 2.0f;
    float halfHeight = height_ / 2.0f;
    auto state = backGroundSprite_->GetStatePtr();

    state.mesh->vertexBufferMap[0].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[1].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[2].position = { halfWidth, halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[3].position = { halfWidth, -halfHeight, 0.0f, 1.0f };
}

void PlayerShootCoolDownBar::CalculateCoolDownBarSpriteVertexPos() {
    float halfWidth = width_ / 2.0f;
    float halfHeight = height_ / 2.0f;
    float ratio = coolDownCurrent_ / coolDownMax_;
    auto state = coolDownBarSprite_->GetStatePtr();

    state.mesh->vertexBufferMap[0].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[1].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[2].position = { -halfWidth + width_ * ratio, halfHeight, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[3].position = { -halfWidth + width_ * ratio, -halfHeight, 0.0f, 1.0f };
}