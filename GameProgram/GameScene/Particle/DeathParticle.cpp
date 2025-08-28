#include "DeathParticle.h"
#include <KashipanEngine.h>
#include <Common/Random.h>
#include <Common/Easings.h>

using namespace KashipanEngine;

DeathParticle::DeathParticle(Model *model, const Vector3 &position) {
    position_ = position;
    // パーティクルの生成
    for (int i = 0; i < particleCount_; i++) {
        // パーティクルの進行方向をランダムに設定
        Vector3 dir = {
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f)
        };
        // パーティクルの回転方向をランダムに設定
        Vector3 rot = {
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f)
        };

        std::unique_ptr<ParticleBlock> particle = std::make_unique<ParticleBlock>(
            model, position, dir, rot, 0.0f, 2.0f, particleLifeTime_, true);
        particleBlocks_.emplace_back(std::move(particle));
    }
}

void DeathParticle::Update() {
    if (!isAlive_) {
        return;
    }

    // 各パーティクルの更新
    for (auto &particle : particleBlocks_) {
        particle->Update();
    }

    elapsedTime_ += Engine::GetDeltaTime();
    float progress = elapsedTime_ / particleLifeTime_;
    
    float scale = Ease::InCubic(progress, 0.5f, 0.0f);
    for (auto &particle : particleBlocks_) {
        particle->SetScale(Vector3(scale));
        Vector3 dir = particle->GetDirection();
        Vector3 pos = {
            Ease::OutCubic(progress, 0.0f, dir.x * 4.0f),
            Ease::OutCubic(progress, 0.0f, dir.y * 4.0f),
            Ease::OutCubic(progress, 0.0f, dir.z * 4.0f)
        };
        particle->SetPosition(position_ + pos);
    }

    if (elapsedTime_ >= particleLifeTime_) {
        isAlive_ = false;
    }
}

void DeathParticle::Draw() {
    // 各パーティクルの描画
    for (auto &particle : particleBlocks_) {
        particle->Draw();
    }
}
