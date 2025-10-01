#pragma once
#include <Objects.h>
#include <list>
#include <memory>
#include <functional>

#include "../Bullets/BaseBullet.h"
#include "../BaseEntity.h"

class EnemyNormal : public BaseEntity {
public:
    EnemyNormal() = delete;
    EnemyNormal(const KashipanEngine::Vector3 &pos, KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera);

    // レベルを設定する関数
    void SetLevel(int level) override;
    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision(Collider *other) override;

    // 更新処理
    void Update() override;
    // 描画処理
    void Draw() override;

private:
    // 移動関数
    void Move();
    // 回転関数
    void Rotate();
};