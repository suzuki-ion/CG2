#pragma once
#include <vector>
#include <string>
#include "Object.h"
#include "Math/AffineMatrix.h"

namespace KashipanEngine {

// 前方宣言
class TextureManager;

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

    /// @brief 親のワールド行列を設定
    /// @param parentWorldMatrix 親のワールド行列
    void SetParentWorldMatrix(const Matrix4x4 &parentWorldMatrix) {
        parentWorldMatrix_ = parentWorldMatrix;
    }

    /// @brief 描画処理
    void Draw();

private:
    /// @brief インデックス数
    UINT indexCount_ = 0;
    /// @brief モデルのマテリアル
    MaterialData materialData_;

    /// @brief 親のワールド行列
    Matrix4x4 parentWorldMatrix_;
};

/// @brief モデルクラス
class Model {
public:
    static void SetTextureManager(TextureManager *textureManager);

    /// @brief 必ずモデルのデータを設定してもらいたいので、デフォルトコンストラクタは削除
    Model() = delete;
    /// @brief Modelのコンストラクタ
    /// @param directoryPath モデルのディレクトリパス
    /// @param fileName モデルのファイル名
    Model(std::string directoryPath, std::string fileName);

    /// @brief 描画処理
    void Draw();

    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    void SetRenderer(Renderer *renderer);

    /// @brief transformへのアクセス
    /// @return モデル全体のtransform
    Transform &GetTransform() {
        return transform;
    }

    /// @brief modelデータへのアクセス
    /// @return モデルデータの参照
    std::vector<ModelData> &GetModels() {
        return models;
    }

private:
    /// @brief モデルデータ全体のtransform
    Transform transform;
    /// @brief ワールド行列
    AffineMatrix worldMatrix;

    /// @brief モデルデータ
    std::vector<ModelData> models;
};

} // namespace KashipanEngine