#pragma once
#include <Objects/Text.h>
#include <Math/Camera.h>

class DamageAmountDisplay {
public:
    DamageAmountDisplay() = delete;
    DamageAmountDisplay(float damageAmount, const KashipanEngine::Vector3 &position, KashipanEngine::Camera *camera, const KashipanEngine::FontData &fontData);
    ~DamageAmountDisplay() = default;

    bool IsActive() const { return isActive_; }
    
    void Update();
    void Draw();

private:
    KashipanEngine::Vector3 startPosition_;
    KashipanEngine::Vector3 endPosition_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Text> damageText_;
    float displayDuration_ = 0.5f;
    float elapsedTime_ = 0.0f;
    bool isActive_ = true;
};