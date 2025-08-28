#pragma once
#include <Objects/Text.h>
#include "../Bullets/BaseBullet.h"

class PlayerBulletTypeText {
public:
    PlayerBulletTypeText() = delete;
    PlayerBulletTypeText(float width, float height);
    ~PlayerBulletTypeText() = default;

    void SetPosition(const KashipanEngine::Vector3 &position) {
        worldTransform_->translate_ = position;
    }
    void SetBulletType(BulletType bullletType);
    void Draw() const;

private:
    float width_;
    float height_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Text> text_;
};