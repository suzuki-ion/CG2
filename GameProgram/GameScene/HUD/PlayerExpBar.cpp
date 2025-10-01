#include "PlayerExpBar.h"

using namespace KashipanEngine;

PlayerExpBar::PlayerExpBar() {
    worldTransform_ = std::make_unique<WorldTransform>();
    backGroundSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    expBarSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");

    worldTransform_->translate_ = position_;
    backGroundSprite_->GetStatePtr().material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    expBarSprite_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 1.0f, 1.0f);

    CalculateBackGroundSpriteVertexPos();
    CalculateExpBarSpriteVertexPos();
}

void PlayerExpBar::Draw() const {
    backGroundSprite_->Draw(*worldTransform_);
    expBarSprite_->Draw(*worldTransform_);
}

void PlayerExpBar::SetExpMax(float expMax) {
    expMax_ = expMax;
    CalculateExpBarSpriteVertexPos();
}

void PlayerExpBar::SetExpCurrent(float expCurrent) {
    expCurrent_ = expCurrent;
    if (expCurrent_ < 0.0f) {
        expCurrent_ = 0.0f;
    }
    if (expCurrent_ > expMax_) {
        expCurrent_ = expMax_;
    }
    CalculateExpBarSpriteVertexPos();
}

void PlayerExpBar::CalculateBackGroundSpriteVertexPos() {
    auto state = backGroundSprite_->GetStatePtr();
    state.mesh->vertexBufferMap[0].position = { 0.0f, height_, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[2].position = { width_, height_, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[3].position = { width_, 0.0f, 0.0f, 1.0f };
}

void PlayerExpBar::CalculateExpBarSpriteVertexPos() {
    float expBarWidth = (expCurrent_ / expMax_) * width_;
    auto state = expBarSprite_->GetStatePtr();
    state.mesh->vertexBufferMap[0].position = { 0.0f, height_, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[2].position = { expBarWidth, height_, 0.0f, 1.0f };
    state.mesh->vertexBufferMap[3].position = { expBarWidth, 0.0f, 0.0f, 1.0f };
}