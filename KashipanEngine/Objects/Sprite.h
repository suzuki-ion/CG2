#pragma once
#include <Common/TextureData.h>
#include "Object.h"

namespace KashipanEngine {

class Sprite : public Object {
public:
    Sprite() = delete;
    Sprite(const std::string &filePath);
    Sprite(const uint32_t textureIndex);
    
    // @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] StatePtr GetStatePtr() override {
        return { mesh_.get(), &transform_, &uvTransform_, &material_, &useTextureIndex_, &normalType_, &pipeLineName_ };
    }
    
    /// @brief テクスチャ設定用関数
    /// @param filePath テクスチャのファイルパス
    void SetTexture(const std::string &filePath);

    /// @brief テクスチャ設定用関数
    /// @param textureIndex テクスチャのインデックス
    void SetTexture(const uint32_t textureIndex);

private:
    /// @brief 初期化処理
    void Initialize();

    /// @brief テクスチャのロード
    /// @param textureData テクスチャデータ
    void LoadTexture(const TextureData &textureData);
};

} // namespace KashipanEngine