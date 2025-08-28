#pragma once
#include <list>
#include "Collider.h"

class CollisionManager {
public:
    void AddCollider(Collider *collider);
    void ClearColliders();
    void Update();

    const std::list<Collider *> &GetColliders() const { return colliders_; }
    int GetCollisionCount() const { return collisionCount_; }

private:
    void CheckCollisionPair(Collider *colliderA, Collider *colliderB);

    std::list<Collider *> colliders_;

    // 衝突数
    int collisionCount_ = 0;
};