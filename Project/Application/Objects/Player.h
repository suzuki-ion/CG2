#pragma once
#include "Objects/Plane.h"
#include "Base/Input.h"

namespace KashipanEngine {

class Player {
public:
    Player(const Vector3 &cameraRotate, int textureIndex);
    ~Player() = default;

    void Update();
    void Draw();

    void SetMoveSpeed(float speed) { moveSpeed_ = speed; }

    // 現在位置の取得
    [[nodiscard]] Vector3 GetPosition() const;

private:
    std::unique_ptr<KashipanEngine::Plane> plane_;
    float moveSpeed_ = 0.1f; // 1フレーム当たり移動速度
};

} // namespace KashipanEngine
