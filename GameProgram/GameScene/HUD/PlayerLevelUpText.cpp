#include "PlayerLevelUpText.h"
#include <KashipanEngine.h>
#include <cmath>

using namespace KashipanEngine;

PlayerLevelUpText::PlayerLevelUpText(Camera *camera) {
    camera_ = camera;
    worldTransform_ = std::make_unique<WorldTransform>();
    levelUpText_ = std::make_unique<Text>(16);
    levelUpText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    levelUpText_->SetText(u8"LEVEL UP!");
    levelUpText_->SetTextAlign(TextAlignX::Center, TextAlignY::Bottom);
    auto textState = levelUpText_->GetStatePtr();
    textState.material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    worldTransform_->scale_ = Vector3(0.5f, 0.5f, 1.0f);
}

void PlayerLevelUpText::SetPosition(const Vector3 &position) {
    if (!isActive_) {
        return;
    }
    Vector3 screenPos = position.Transform(camera_->GetViewMatrix() * camera_->GetProjectionMatrix());
    screenPos.x = (screenPos.x + 1.0f) * 0.5f * 1920.0f;
    screenPos.y = (1.0f - screenPos.y) * 0.5f * 1080.0f - 64.0f;
    worldTransform_->translate_ = screenPos;
}

void PlayerLevelUpText::Update() {
    if (elapsedTime_ > displayDuration_) {
        isActive_ = false;
        return;
    }

    elapsedTime_ += Engine::GetDeltaTime();
    float blinking = std::fmodf(elapsedTime_, 0.5f);
    if (blinking < 0.25f) {
        levelUpText_->GetStatePtr().material->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        levelUpText_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    if (elapsedTime_ > fadeOutStartTime_) {
        float t = (elapsedTime_ - fadeOutStartTime_) / (displayDuration_ - fadeOutStartTime_);
        levelUpText_->GetStatePtr().material->color.w = 1.0f - t;
    }
}

void PlayerLevelUpText::Draw() {
    if (!isActive_) {
        return;
    }
    levelUpText_->SetPipelineName("Object3d.Solid.BlendNormal");
    levelUpText_->Draw(*worldTransform_);
    levelUpText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    levelUpText_->Draw(*worldTransform_);
}