#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Sprite : public Object {
public:
    Sprite();

    /// @brief 描画処理
    void Draw();
};

} // namespace KashipanEngine