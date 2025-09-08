#pragma once
#include "ParticleBlock.h"
#include <list>

class DeathParticle {
public:
    /// @brief コンストラクタ
    /// @param model パーティクルのモデル
    /// @param position パーティクルの初期位置
    DeathParticle(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position);
    ~DeathParticle() = default;

    bool IsAlive() const { return isAlive_; }

    void Update();
    void Draw();

private:
    // パーティクルの発生位置
    KashipanEngine::Vector3 position_;
    // パーティクルブロックのリスト
    std::list<std::unique_ptr<ParticleBlock>> particleBlocks_;
    // パーティクルの数
    int particleCount_ = 8;
    // パーティクルの生存時間(秒)
    float particleLifeTime_ = 1.0f;
    // 現在の経過時間(秒)
    float elapsedTime_ = 0.0f;
    // パーティクル生存フラグ
    bool isAlive_ = true;
};