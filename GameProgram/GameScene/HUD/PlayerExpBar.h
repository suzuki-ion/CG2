#pragma once
#include <Objects/Sprite.h>

class PlayerExpBar {
public:
    PlayerExpBar();
    ~PlayerExpBar() = default;

    void Draw() const;

    void SetExpMax(float expMax);
    void SetExpCurrent(float expCurrent);

    void SetPosition(const KashipanEngine::Vector2 &position) { position_ = position; }
    void SetWidth(float width) { width_ = width; }
    void SetHeight(float height) { height_ = height; }

    float GetExpMax() const { return expMax_; }
    float GetExpCurrent() const { return expCurrent_; }

private:
    void CalculateBackGroundSpriteVertexPos();
    void CalculateExpBarSpriteVertexPos();

    void AnimationUpdate();

    // EXPバーのワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    
    float expMax_ = 100.0f;
    float expCurrent_ = 0.0f;

    // ゲージの位置(左上)
    KashipanEngine::Vector2 position_ = { 16.0f, 120.0f };
    float width_ = 536.0f;  // ゲージの幅
    float height_ = 16.0f;  // ゲージの高さ

    std::unique_ptr<KashipanEngine::Sprite> backGroundSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> expBarSprite_ = nullptr;
};