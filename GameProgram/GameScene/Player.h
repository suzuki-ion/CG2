#pragma once
#include <Objects.h>
#include <list>
#include <memory>
#include <functional>

#include "Bullets/BaseBullet.h"
#include "BaseEntity.h"

class Player : public BaseEntity {
public:
    Player() = delete;
    Player(KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera);

    void SetMoveDirection(const KashipanEngine::Vector3 &dir) {
        moveDirection_ = dir;
    }

    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision(Collider *other) override;

    // 更新処理
    void Update() override;
    // 描画処理
    void Draw() override;

private:
    // ホーミング弾発射関数
    bool ShootHomingBullet();
    // レベルアップ用関数
    void LevelUp();

    // 移動関数
    void Move();
    // 回転関数
    void Rotate();
    // 移動制限用関数
    void LimitPosition();

    // ターゲット最大数
    int maxTargetCount_ = 3;
    // レベルアップフラグ
    bool isLevelUp_ = false;
};