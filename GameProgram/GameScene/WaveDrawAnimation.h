#pragma once
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include "../BaseAnimation.h"

class WaveDrawAnimation : public BaseAnimation {
public:
    WaveDrawAnimation(float width, float height);
    ~WaveDrawAnimation() override = default;

    void SetWaveNumber(int waveNumber, bool isLastWave, bool isClearWave);

    float GetNoneTime() const { return noneTime_; }
    int GetWaveNumber() const { return waveNumber_; }
    bool IsLastWave() const { return isLastWave_; }
    bool IsClearWave() const { return isClearWave_; }
    bool IsAnimationStartTrigger() const { return isAnimationStartTrigger_; }

    void Update() override;
    void Draw() override;

private:
    void Reset();
    void ResetNormal();
    void ResetLast();
    void ResetClear();

    float width_;
    float height_;

    float noneTime_ = 0.0f;
    float prevElapsedTime_ = 0.0f;

    int waveNumber_ = 1;
    bool isLastWave_ = false;
    bool isClearWave_ = false;
    bool isAnimationStartTrigger_ = false;
    std::unique_ptr<KashipanEngine::WorldTransform> textWorldTransform_;
    std::unique_ptr<KashipanEngine::WorldTransform> textShakeWorldTransform_;
    std::unique_ptr<KashipanEngine::Sprite> rectSpriteTop_;
    std::unique_ptr<KashipanEngine::Sprite> rectSpriteBottom_;
    std::unique_ptr<KashipanEngine::Text> waveText_;
};