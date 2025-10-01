#include "FadeIn.h"
#include <KashipanEngine.h>
#include <Common/Easings.h>

using namespace KashipanEngine;

FadeIn::FadeIn(float duration, const Vector4 &color, float width, float height)
    : BaseAnimation(duration) {
    easingFunction_ = Ease::InOutSine;
    fadeInSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    auto spriteState = fadeInSprite_->GetStatePtr();
    spriteState.material->color = color;
    spriteState.material->color.w = 1.0f;
    spriteState.transform->scale = Vector3(width, height, 1.0f);
}

void FadeIn::Update() {
    UpdateElapsedTime();
    float t = elapsedTime_ / duration_;
    fadeInSprite_->GetStatePtr().material->color.w = easingFunction_(t, 1.0f, 0.0f);
}

void FadeIn::Draw() {
    fadeInSprite_->Draw();
}
