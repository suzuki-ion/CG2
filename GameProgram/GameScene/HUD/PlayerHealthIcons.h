#pragma once
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include <Common/Easings.h>
#include <functional>

class PlayerHealthIcons {
public:
    PlayerHealthIcons();
    ~PlayerHealthIcons() = default;

    void Update();
    void Draw();

    void SetHealthMax(int healthMax);
    void SetHealthCurrent(int healthCurrent);

    int GetHealthMax() const { return healthMax_; }
    int GetHealthCurrent() const { return healthCurrent_; }

private:
    void AnimationUpdate();

    // アイコン用のワールド変換データ
    std::vector<KashipanEngine::WorldTransform> worldTransforms_;
    // シェイクアニメーション用のワールド変換データ
    std::vector<KashipanEngine::WorldTransform> shakeWorldTransforms_;

    int healthMax_ = 10;
    int healthCurrent_ = 10;

    std::unique_ptr<KashipanEngine::Sprite> healthIconSprite_ = nullptr;
};