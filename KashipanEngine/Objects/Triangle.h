#pragma once
#include "Object.h"

namespace KashipanEngine {

class Triangle : public Object {
public:
    Triangle();

    /// @brief 描画処理
    void Draw();
};

} // namespace KashipanEngine