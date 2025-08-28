#pragma once
#include "Object.h"

namespace KashipanEngine {

class Triangle : public Object {
public:
    Triangle();

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);
};

} // namespace KashipanEngine