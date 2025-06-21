#pragma once
#include "Objects.h"

class Skydome {
public:
    // 初期化
    void Initialize(KashipanEngine::Model *model);

    // 更新
    void Update();

    // 描画
    void Draw();

private:
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    KashipanEngine::Model *model_ = nullptr;
};
