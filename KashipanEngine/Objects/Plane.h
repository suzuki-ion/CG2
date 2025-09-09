#pragma once
#include "Objects/Object.h"

namespace KashipanEngine {

class Plane : public Object {
public:
    Plane();

    /// @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] StatePtr GetStatePtr() override {
        return { mesh_.get(), &transform_, &uvTransform_, &material_, &useTextureIndex_, &normalType_, &pipeLineName_};
    }

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    void Draw(WorldTransform &worldTransform);
};

} // namespace KashipanEngine