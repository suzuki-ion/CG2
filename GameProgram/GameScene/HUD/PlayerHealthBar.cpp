#include "PlayerHealthBar.h"
#include <KashipanEngine.h>
#include <Common/Random.h>
#include <format>

using namespace KashipanEngine;

PlayerHealthBar::PlayerHealthBar() {
    outlineSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    outerEdgeSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    outerEdgeSprite_->GetStatePtr().material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    currentHealthSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    lostHealthSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    lostHealthSprite_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    
    healthTextGreen_ = std::make_unique<Text>(32);
    healthTextGreen_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    healthTextGreen_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    healthTextGreen_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    healthTextRed_ = std::make_unique<Text>(32);
    healthTextRed_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    healthTextRed_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    healthTextRed_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    
    CalculateOutlineSpriteVertexPos();
    CalculateOuterEdgeSpriteVertexPos();
    CalculateCurrentHealthSpriteVertexPos();
    CalculateLostHealthSpriteVertexPos();
    CalculateHealthColor();
    SetHealthText();

    worldTransform_ = std::make_unique<WorldTransform>();
    textWorldTransform_ = std::make_unique<WorldTransform>();
    shakeWorldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->parentTransform_ = shakeWorldTransform_.get();
    textWorldTransform_->parentTransform_ = worldTransform_.get();
    worldTransform_->translate_ = {
        position_.x,
        position_.y,
        0.0f
    };
    textWorldTransform_->translate_ = {
        (width_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f) / 2.0f,
        (height_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f) / 2.0f,
        0.0f
    };
    textWorldTransform_->scale_ = Vector3(0.75f, 0.75f, 1.0f);
}

void PlayerHealthBar::Update() {
    shakeWorldTransform_->TransferMatrix();
    AnimationUpdate();
}

void PlayerHealthBar::Draw() {
    outlineSprite_->Draw(*worldTransform_);
    outerEdgeSprite_->Draw(*worldTransform_);
    lostHealthSprite_->Draw(*worldTransform_);
    currentHealthSprite_->Draw(*worldTransform_);

    healthTextRed_->SetPipelineName("Object3d.Solid.BlendNormal");
    healthTextRed_->Draw(*textWorldTransform_);
    healthTextRed_->SetPipelineName("Object3d.Solid.BlendMultiply");
    healthTextRed_->Draw(*textWorldTransform_);

    healthTextGreen_->SetPipelineName("Object3d.Solid.BlendNormal");
    healthTextGreen_->Draw(*textWorldTransform_);
    healthTextGreen_->SetPipelineName("Object3d.Solid.BlendMultiply");
    healthTextGreen_->Draw(*textWorldTransform_);
}

void PlayerHealthBar::SetHealthMax(float healthMax) {
    healthMax_ = healthMax;
    if (healthCurrent_ > healthMax_) {
        healthCurrent_ = healthMax_;
    }
    SetHealthText();
    CalculateCurrentHealthSpriteVertexPos();
    CalculateHealthColor();
}

void PlayerHealthBar::SetHealthCurrent(float healthCurrent) {
    healthLost_ = healthCurrent_ - healthCurrent;
    healthCurrent_ = healthCurrent;
    if (healthCurrent_ < 0.0f) {
        healthCurrent_ = 0.0f;
    }

    SetHealthText();
    CalculateCurrentHealthSpriteVertexPos();
    CalculateHealthColor();

    // 失った体力がプラスの場合はアニメーションの準備
    if (healthLost_ > 0.0f) {
        healthLostStart_ = healthCurrent_ + healthLost_;
        healthLost_ = healthLostStart_;
        healthLostEnd_ = healthCurrent_;
        healthLostAnimationTimer_ = 0.0f;
        isHealthLostAnimation_ = true;
        CalculateLostHealthSpriteVertexPos();
    }
}

void PlayerHealthBar::CalculateOutlineSpriteVertexPos() {
    auto outlineSpriteState = outlineSprite_->GetStatePtr();
    auto *outlineSpriteMesh = outlineSpriteState.mesh;
    Vector2 outlineSpriteLeftTop = { 0.0f, 0.0f };
    Vector2 outlineSpriteRightBottom = {
        width_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f,
        height_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f,
    };
    outlineSpriteMesh->vertexBufferMap[0].position = { outlineSpriteLeftTop.x, outlineSpriteRightBottom.y, 0.0f, 1.0f };
    outlineSpriteMesh->vertexBufferMap[1].position = { outlineSpriteLeftTop.x, outlineSpriteLeftTop.y, 0.0f, 1.0f };
    outlineSpriteMesh->vertexBufferMap[2].position = { outlineSpriteRightBottom.x, outlineSpriteRightBottom.y, 0.0f, 1.0f };
    outlineSpriteMesh->vertexBufferMap[3].position = { outlineSpriteRightBottom.x, outlineSpriteLeftTop.y, 0.0f, 1.0f };
}

void PlayerHealthBar::CalculateOuterEdgeSpriteVertexPos() {
    auto outerEdgeSpriteState = outerEdgeSprite_->GetStatePtr();
    auto *outerEdgeSpriteMesh = outerEdgeSpriteState.mesh;
    Vector2 outerEdgeSpriteLeftTop = {
        outlineThickness_,
        outlineThickness_,
    };
    Vector2 outerEdgeSpriteRightBottom = {
        outlineThickness_ + width_ + outerEdgeThickness_ * 2.0f,
        outlineThickness_ + height_ + outerEdgeThickness_ * 2.0f,
    };
    outerEdgeSpriteMesh->vertexBufferMap[0].position = { outerEdgeSpriteLeftTop.x, outerEdgeSpriteRightBottom.y, 0.0f, 1.0f };
    outerEdgeSpriteMesh->vertexBufferMap[1].position = { outerEdgeSpriteLeftTop.x, outerEdgeSpriteLeftTop.y, 0.0f, 1.0f };
    outerEdgeSpriteMesh->vertexBufferMap[2].position = { outerEdgeSpriteRightBottom.x, outerEdgeSpriteRightBottom.y, 0.0f, 1.0f };
    outerEdgeSpriteMesh->vertexBufferMap[3].position = { outerEdgeSpriteRightBottom.x, outerEdgeSpriteLeftTop.y, 0.0f, 1.0f };
}

void PlayerHealthBar::CalculateCurrentHealthSpriteVertexPos() {
    auto currentHealthSpriteState = currentHealthSprite_->GetStatePtr();
    auto *currentHealthSpriteMesh = currentHealthSpriteState.mesh;
    Vector2 currentHealthSpriteLeftTop = {
        outlineThickness_ + outerEdgeThickness_,
        outlineThickness_ + outerEdgeThickness_,
    };
    Vector2 currentHealthSpriteRightBottom = {
        outlineThickness_ + outerEdgeThickness_ + (width_ * (healthCurrent_ / healthMax_)),
        outlineThickness_ + outerEdgeThickness_ + height_,
    };
    currentHealthSpriteMesh->vertexBufferMap[0].position = { currentHealthSpriteLeftTop.x, currentHealthSpriteRightBottom.y, 0.0f, 1.0f };
    currentHealthSpriteMesh->vertexBufferMap[1].position = { currentHealthSpriteLeftTop.x, currentHealthSpriteLeftTop.y, 0.0f, 1.0f };
    currentHealthSpriteMesh->vertexBufferMap[2].position = { currentHealthSpriteRightBottom.x, currentHealthSpriteRightBottom.y, 0.0f, 1.0f };
    currentHealthSpriteMesh->vertexBufferMap[3].position = { currentHealthSpriteRightBottom.x, currentHealthSpriteLeftTop.y, 0.0f, 1.0f };
}

void PlayerHealthBar::CalculateLostHealthSpriteVertexPos() {
    auto lostHealthSpriteState = lostHealthSprite_->GetStatePtr();
    auto *lostHealthSpriteMesh = lostHealthSpriteState.mesh;
    Vector2 lostHealthSpriteLeftTop = {
        outlineThickness_ + outerEdgeThickness_,
        outlineThickness_ + outerEdgeThickness_,
    };
    Vector2 lostHealthSpriteRightBottom = {
        outlineThickness_ + outerEdgeThickness_ + (width_ * (healthLost_ / healthMax_)),
        outlineThickness_ + outerEdgeThickness_ + height_,
    };
    lostHealthSpriteMesh->vertexBufferMap[0].position = { lostHealthSpriteLeftTop.x, lostHealthSpriteRightBottom.y, 0.0f, 1.0f };
    lostHealthSpriteMesh->vertexBufferMap[1].position = { lostHealthSpriteLeftTop.x, lostHealthSpriteLeftTop.y, 0.0f, 1.0f };
    lostHealthSpriteMesh->vertexBufferMap[2].position = { lostHealthSpriteRightBottom.x, lostHealthSpriteRightBottom.y, 0.0f, 1.0f };
    lostHealthSpriteMesh->vertexBufferMap[3].position = { lostHealthSpriteRightBottom.x, lostHealthSpriteLeftTop.y, 0.0f, 1.0f };
}

void PlayerHealthBar::CalculateHealthTextPos() {
    textWorldTransform_->translate_ = {
        position_.x + (width_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f) / 2.0f,
        position_.y + (height_ + outerEdgeThickness_ * 2.0f + outlineThickness_ * 2.0f) / 2.0f,
        0.0f
    };
}

void PlayerHealthBar::CalculateHealthColor() {
    healthColor_ = Vector4::Lerp(healthColorMin_, healthColorMax_, healthCurrent_ / healthMax_);
    outlineSprite_->GetStatePtr().material->color = healthColor_;
    currentHealthSprite_->GetStatePtr().material->color = healthColor_;
    healthTextGreen_->GetStatePtr().material->color.w = healthColor_.y;
}

void PlayerHealthBar::SetHealthText() {
    std::string text = std::format("HP {:.2f} / {:.2f}", healthCurrent_, healthMax_);
    healthTextGreen_->SetText(std::u8string(text.begin(), text.end()));
    healthTextRed_->SetText(std::u8string(text.begin(), text.end()));
}

void PlayerHealthBar::AnimationUpdate() {
    if (!isHealthLostAnimation_) {
        return;
    }

    // 体力が減るアニメーションが始まるまではシェイク
    if (healthLostAnimationTimer_ < healthLostAnimationDelay_) {
        float shakeProgress = healthLostAnimationTimer_ / healthLostAnimationDelay_;
        float offsetX = GetRandomFloat(-8.0f, 8.0f) * (1.0f - shakeProgress);
        float offsetY = GetRandomFloat(-8.0f, 8.0f) * (1.0f - shakeProgress);
        shakeWorldTransform_->translate_ = { offsetX, offsetY, 0.0f };

    } else {
        // シェイク終了
        shakeWorldTransform_->translate_ = { 0.0f, 0.0f, 0.0f };

        // 体力が減るアニメーション
        float animationProgress = (healthLostAnimationTimer_ - healthLostAnimationDelay_) / healthLostAnimationDuration_;
        healthLost_ = healthLostEasingFunction_(animationProgress, healthLostStart_, healthLostEnd_);
        CalculateLostHealthSpriteVertexPos();
    }
    
    healthLostAnimationTimer_ += Engine::GetDeltaTime();
    if (healthLostAnimationTimer_ >= healthLostAnimationDuration_ + healthLostAnimationDelay_) {
        healthLostAnimationTimer_ = healthLostAnimationDuration_;
        isHealthLostAnimation_ = false;
    }
}
