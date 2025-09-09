#pragma once
#include "Objects/Object.h"

namespace KashipanEngine {

class Cube : public Object {
public:
    Cube();

    /// @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] StatePtr GetStatePtr() override {
        return { mesh_.get(), &transform_, &uvTransform_, &material_, &useTextureIndex_, &normalType_, &pipeLineName_ };
    }
};

} // namespace KashipanEngine