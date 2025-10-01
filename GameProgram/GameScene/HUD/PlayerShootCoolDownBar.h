#pragma once
#include <Objects/Sprite.h>
#include <Math/Camera.h>

class PlayerShootCoolDownBar {
public:
    PlayerShootCoolDownBar() = delete;
    PlayerShootCoolDownBar(KashipanEngine::Camera *camera);
    ~PlayerShootCoolDownBar() = default;

    void SetPosition(const KashipanEngine::Vector3 &position);
    void SetCoolDownMax(float coolDownMax);
    void SetCoolDownCurrent(float coolDownCurrent);
    void Draw();

private:
    void CalculateBackGroundSpriteVertexPos();
    void CalculateCoolDownBarSpriteVertexPos();

    float coolDownMax_ = 100.0f;
    float coolDownCurrent_ = 0.0f;

    float width_ = 128.0f;  // ゲージの幅
    float height_ = 8.0f;  // ゲージの高さ

    KashipanEngine::Camera *camera_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Sprite> backGroundSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> coolDownBarSprite_ = nullptr;
};