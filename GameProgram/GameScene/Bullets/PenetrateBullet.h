#pragma once
#include "BaseBullet.h"

class PenetrateBullet : public BaseBullet {
public:
    PenetrateBullet(KashipanEngine::Model *bulletModel, const std::bitset<8> &collisionAttributes, float radius, float attackPower, float speed, float interval)
        : BaseBullet(bulletModel, collisionAttributes, radius, 0.1f, 1.5f, 0.5f, attackPower, speed, interval) {
    }
    ~PenetrateBullet() override = default;

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

private:
    float rotateSpeed_ = 4.0f;
};