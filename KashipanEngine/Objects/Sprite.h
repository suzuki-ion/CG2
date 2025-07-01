#pragma once
#include <Common/TextureData.h>
#include "Object.h"

namespace KashipanEngine {

struct Sprite : public Object {
public:
    Sprite() = delete;
    Sprite(const std::string &filePath);
    Sprite(const uint32_t textureIndex);

    /// @brief テクスチャ設定用関数
    /// @param filePath テクスチャのファイルパス
    void SetTexture(const std::string &filePath);

    /// @brief テクスチャ設定用関数
    /// @param textureIndex テクスチャのインデックス
    void SetTexture(const uint32_t textureIndex);

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

private:
    /// @brief 初期化処理
    void Initialize();

    /// @brief テクスチャのロード
    /// @param textureData テクスチャデータ
    void LoadTexture(const TextureData &textureData);
};

} // namespace KashipanEngine