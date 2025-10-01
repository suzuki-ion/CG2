#pragma once
#include "../BaseAnimation.h"
#include <Objects/Sprite.h>
#include <functional>

class FadeOut : public BaseAnimation {
public:
    FadeOut(float duration, const KashipanEngine::Vector4 &color, float width, float height);

    void Update() override;
    void Draw() override;

private:
    std::function<float(float, float, float)> easingFunction_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> fadeOutSprite_;
};