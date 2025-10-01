#pragma once
#include <Objects/Text.h>
#include <Math/Camera.h>

class PlayerLevelUpText {
public:
    PlayerLevelUpText() = delete;
    PlayerLevelUpText(KashipanEngine::Camera *camera);
    ~PlayerLevelUpText() = default;

    void Show() {
        isActive_ = true;
        elapsedTime_ = 0.0f;
    }
    void SetPosition(const KashipanEngine::Vector3 &position);
    bool IsActive() const { return isActive_; }

    void Update();
    void Draw();

private:
    KashipanEngine::Camera *camera_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Text> levelUpText_;
    float displayDuration_ = 3.0f;
    float fadeOutStartTime_ = 2.0f;
    float elapsedTime_ = 0.0f;
    bool isActive_ = false;
};