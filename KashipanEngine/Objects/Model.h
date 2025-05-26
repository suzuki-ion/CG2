#pragma once
#include <vector>
#include <string>
#include "Object.h"

namespace KashipanEngine {

// 前方宣言
class TextureManager;

/// @brief モデルのマテリアルデータ
struct MaterialData {
    std::string textureFilePath;
};

/// @brief モデルデータ
struct ModelData : public Object {
    /// @brief モデルの頂点データ
    std::vector<VertexData> vertices;
    /// @brief インデックス数
    UINT indexCount = 0;
    /// @brief モデルのマテリアル
    MaterialData materialData;
};

struct Model {
    /// @brief 必ずモデルのデータを設定してもらいたいので、デフォルトコンストラクタは削除
    Model() = delete;
    /// @brief Modelのコンストラクタ
    /// @param directoryPath モデルのディレクトリパス
    /// @param fileName モデルのファイル名
    /// @param textureManager テクスチャ管理クラスへのポインタ
    Model(std::string directoryPath, std::string fileName, TextureManager *textureManager);

    /// @brief モデルデータ
    std::vector<ModelData> models;
};

} // namespace KashipanEngine