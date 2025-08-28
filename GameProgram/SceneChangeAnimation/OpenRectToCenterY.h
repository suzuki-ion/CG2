#pragma once
#include "../BaseAnimation.h"
#include <Objects/Sprite.h>
#include <functional>

class OpenRectToCenterY : public BaseAnimation {
public:
    OpenRectToCenterY(float duration, float width, float height);

    void Update() override;
    void Draw() override;

private:
    void CalculateTopRectGreenVertexPos();
    void CalculateBottomRectGreenVertexPos();
    void CalculateTopRectBlackVertexPos();
    void CalculateBottomRectBlackVertexPos();

    std::function<float(float, float, float)> easingFunction_ = nullptr;

    std::unique_ptr<KashipanEngine::Sprite> topRectGreen_;
    std::unique_ptr<KashipanEngine::Sprite> bottomRectGreen_;
    std::unique_ptr<KashipanEngine::Sprite> topRectBlack_;
    std::unique_ptr<KashipanEngine::Sprite> bottomRectBlack_;

    float greenTopPosY_;
    float greenBottomPosY_;
    float blackTopPosY_;
    float blackBottomPosY_;

    float startAnimationTimeToGreen_;

    float width_ = 0.0f;
    float height_ = 0.0f;
};