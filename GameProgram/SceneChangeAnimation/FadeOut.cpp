#include "FadeOut.h"
#include <KashipanEngine.h>
#include <Common/Easings.h>

using namespace KashipanEngine;

FadeOut::FadeOut(float duration, const Vector4 &color, float width, float height)
    : BaseAnimation(duration) {
    easingFunction_ = Ease::InOutSine;
    fadeOutSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    auto spriteState = fadeOutSprite_->GetStatePtr();
    spriteState.material->color = color;
    spriteState.material->color.w = 0.0f;
    spriteState.transform->scale = Vector3(width, height, 1.0f);
}

void FadeOut::Update() {
    UpdateElapsedTime();
    float t = elapsedTime_ / duration_;
    fadeOutSprite_->GetStatePtr().material->color.w = easingFunction_(t, 0.0f, 1.0f);
}

void FadeOut::Draw() {
    fadeOutSprite_->Draw();
}
