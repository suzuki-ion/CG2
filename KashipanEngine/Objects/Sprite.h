#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Sprite : public Object {
public:
    Sprite() = delete;
    Sprite(const std::string &filePath);
    Sprite(const uint32_t textureIndex);

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);
};

} // namespace KashipanEngine