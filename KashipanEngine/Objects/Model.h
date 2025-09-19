#pragma once
#include <vector>
#include <string>
#include "Objects/Object.h"
#include "Math/AffineMatrix.h"

namespace KashipanEngine {

// 前方宣言
class Texture;

/// @brief モデルのマテリアルデータ
struct MaterialData {
    std::string textureFilePath;
};

/// @brief モデルデータ
class ModelData : public Object {
public:
    ModelData() = default;

    /// @brief モデルデータの作成
    /// @param vertexData 頂点データ
    /// @param indexData インデックスデータ
    /// @param materialData モデルのマテリアルデータ
    void CreateData(std::vector<VertexData> &vertexData, std::vector<uint32_t> &indexData, MaterialData &materialData);

    /// @brief メッシュの存在確認
    /// @return メッシュが存在するならtrue、存在しないならfalse
    bool isMeshExist() const {
        return mesh_.get() != nullptr;
    }

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

private:
    /// @brief インデックス数
    UINT indexCount_ = 0;
    /// @brief モデルのマテリアル
    MaterialData materialData_;
};

/// @brief モデルクラス
class Model : public Object{
public:
    /// @brief 必ずモデルのデータを設定してもらいたいので、デフォルトコンストラクタは削除
    Model() = delete;
    /// @brief Modelのコンストラクタ
    /// @param directoryPath モデルのディレクトリパス
    /// @param fileName モデルのファイル名
    Model(std::string directoryPath, std::string fileName);

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    void SetRenderer(Renderer *renderer) override;

    /// @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] StatePtr GetStatePtr() override {
        return { nullptr, &transform_, nullptr, &material_, nullptr, nullptr, &pipeLineName_ };
    }

    /// @brief modelデータへのアクセス
    /// @return モデルデータの参照
    const std::vector<std::unique_ptr<ModelData>> &GetModels() {
        return models_;
    }

private:
    /// @brief モデルデータ
    std::vector<std::unique_ptr<ModelData>> models_;
};

} // namespace KashipanEngine