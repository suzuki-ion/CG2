#pragma once
#include "BaseBullet.h"

class NormalBullet : public BaseBullet {
public:
    NormalBullet(KashipanEngine::Sprite *bulletSprite, const std::bitset<8> &collisionAttributes, float radius, int attackPower, float speed, float interval)
        : BaseBullet(bulletSprite, collisionAttributes, radius, 1, 8.0f, 1.0f, attackPower, speed, interval) {}
    ~NormalBullet() override = default;

    void Update() override;
    void Draw() override;

    void OnCollision(Collider *other) override;
    const KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
};