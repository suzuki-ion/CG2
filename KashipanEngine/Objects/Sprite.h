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

    /// @brief ピボットの設定
    /// @param pivot ピボット(0.0~1.0)
    void SetPivot(const Vector2 &pivot);

private:
    /// @brief 初期化処理
    void Initialize();

    /// @brief ピボットによる頂点位置Xの調整
    void AdjustVertexPosX(float newPivotX);
    /// @brief ピボットによる頂点位置Yの調整
    void AdjustVertexPosY(float newPivotY);

    /// @brief テクスチャのロード
    /// @param textureData テクスチャデータ
    void LoadTexture(const TextureData &textureData);

    /// @brief スプライトの幅
    float width_ = 0.0f;
    /// @brief スプライトの高さ
    float height_ = 0.0f;
    /// @brief ピボット(0.0~1.0)
    Vector2 pivot_ = { 0.5f, 0.5f };
};

} // namespace KashipanEngine