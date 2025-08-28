#pragma once
#include <Objects/Text.h>
#include "../BaseAnimation.h"
#include "ResultParameters.h"

class ResultDrawAnimation : public BaseAnimation {
public:
    ResultDrawAnimation(float width, float height);
    ~ResultDrawAnimation() override = default;

    void SetResultParameters(const ResultParameters &params);

    void Update() override;
    void Draw() override;

private:
    void Reset();
    float width_;
    float height_;
    ResultParameters params_;
    std::unique_ptr<KashipanEngine::Text> resultLogoText_;
    std::unique_ptr<KashipanEngine::Text> killCountText_;
    std::unique_ptr<KashipanEngine::Text> totalDamageText_;
    std::unique_ptr<KashipanEngine::Text> damageTakenText_;
};