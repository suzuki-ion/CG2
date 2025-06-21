#pragma once
#include "Objects.h"
#include <array>

// 前方宣言
class Player;

class DeathParticle {
public:
    // パーティクルの個数
    static inline const uint32_t kNumParticle = 8;

    void Initialize(KashipanEngine::Model *model);

    // 更新
    void Update();

    // 描画
    void Draw();

    // パーティクル発生
    void Emit(const KashipanEngine::Vector3 &position);

    // 終了フラグの取得
    bool IsFinished() const { return isFinished_; }

private:
    // モデル
    KashipanEngine::Model *model_ = nullptr;

    // ワールド変換データ
    std::array<KashipanEngine::WorldTransform, kNumParticle> worldTransforms_;

    // 終了フラグ
    bool isFinished_ = true;
    // 経過時間カウント
    float counter_ = 0.0f;

    // 色の数値
    KashipanEngine::Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};
