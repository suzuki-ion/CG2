#include "DamageAmountDisplay.h"
#include <KashipanEngine.h>
#include <Common/Easings.h>
#include <format>

using namespace KashipanEngine;

DamageAmountDisplay::DamageAmountDisplay(float damageAmount, const KashipanEngine::Vector3 &position, Camera *camera, const FontData &fontData) {
    worldTransform_ = std::make_unique<WorldTransform>();
    damageText_ = std::make_unique<Text>(8);
    damageText_->SetFont(fontData);
    std::string damageStr = std::format("-{:.2f}", damageAmount);
    damageText_->SetText(std::u8string(damageStr.begin(), damageStr.end()));
    damageText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    auto textState = damageText_->GetStatePtr();
    textState.material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // スクリーン座標に変換
    Vector3 screenPos = position.Transform(camera->GetViewMatrix() * camera->GetProjectionMatrix());
    screenPos.x = (screenPos.x + 1.0f) * 0.5f * 1920.0f;
    screenPos.y = (1.0f - screenPos.y) * 0.5f * 1080.0f;
    
    startPosition_ = screenPos;
    endPosition_ = screenPos + Vector3(0.0f, 64.0f, 0.0f);
    worldTransform_->translate_ = startPosition_;
    worldTransform_->scale_ = Vector3(0.5f, 0.5f, 1.0f);
}

void DamageAmountDisplay::Update() {
    // 途中から下がるようにする
    const float downStartTime = 0.2f;
    // 上がる高さ
    const float upHeight = 16.0f;

    elapsedTime_ += Engine::GetDeltaTime();
    auto textState = damageText_->GetStatePtr();
    if (elapsedTime_ < downStartTime) {
        float t = elapsedTime_ / downStartTime;
        float y = Ease::OutCubic(t, startPosition_.y, startPosition_.y - upHeight);
        worldTransform_->translate_.y = y;
    
    } else if (elapsedTime_ < displayDuration_) {
        float t = (elapsedTime_ - downStartTime) / (displayDuration_ - downStartTime);
        float y = Ease::InCubic(t, startPosition_.y - upHeight, endPosition_.y);
        worldTransform_->translate_.y = y;
        textState.material->color.w = (1.0f - t);
    
    } else {
        isActive_ = false;
    }
}

void DamageAmountDisplay::Draw() {
    damageText_->SetPipelineName("Object3d.Solid.BlendNormal");
    damageText_->Draw(*worldTransform_);
    damageText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    damageText_->Draw(*worldTransform_);
}