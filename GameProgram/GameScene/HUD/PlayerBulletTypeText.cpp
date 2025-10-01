#include "PlayerBulletTypeText.h"

using namespace KashipanEngine;

PlayerBulletTypeText::PlayerBulletTypeText(float width, float height) {
    width_ = width;
    height_ = height;

    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_.x = 16.0f;
    worldTransform_->translate_.y = height_ - 16.0f;
    text_ = std::make_unique<Text>(16);
    text_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    text_->SetText(u8"Bullet: None");
    text_->SetTextAlign(TextAlignX::Left, TextAlignY::Bottom);
    auto textState = text_->GetStatePtr();
    textState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void PlayerBulletTypeText::SetBulletType(BulletType bullletType) {
    switch (bullletType) {
        case BulletType::kNormal:
            text_->SetText(u8"Bullet: Normal");
            break;
        case BulletType::kHoming:
            text_->SetText(u8"Bullet: Homing");
            break;
        case BulletType::kPenetrate:
            text_->SetText(u8"Bullet: Penetrate");
            break;
        default:
            text_->SetText(u8"Bullet: None");
            break;
    }
}

void PlayerBulletTypeText::Draw() const {
    text_->SetPipelineName("Object3d.Solid.BlendNormal");
    text_->Draw(*worldTransform_);
    text_->SetPipelineName("Object3d.Solid.BlendMultiply");
    text_->Draw(*worldTransform_);
}