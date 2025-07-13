#pragma once
#include "WorldTransform.h"
#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "Common/TransformationMatrix.h"
#include "Common/Material.h"
#include "3d/PrimitiveDrawer.h"

namespace KashipanEngine {

// 前方宣言
class Renderer;

enum NormalType {
    kNormalTypeVertex,
    kNormalTypeFace,
};

class Object {
public:
    struct StatePtr {
        Mesh<VertexData> *mesh = nullptr;
        Transform *transform = nullptr;
        Transform *uvTransform = nullptr;
        Material *material = nullptr;
        int *useTextureIndex = nullptr;
        NormalType *normalType = nullptr;
        FillMode *fillMode = nullptr;
    };

    Object() = default;
    Object(Object &&other) noexcept;

    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }

    /// @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] virtual StatePtr GetStatePtr() {
        return { nullptr, &transform_, &uvTransform_, &material_, &useTextureIndex_, &normalType_, &fillMode_};
    }
    
protected:
    //==================================================
    // メンバ関数
    //==================================================

    /// @brief オブジェクトの生成
    /// @param vertexCount 頂点数
    /// @param indexCount インデックス数
    void Create(UINT vertexCount, UINT indexCount);

    /// @brief オブジェクト共通の描画処理
    void DrawCommon();

    /// @brief オブジェクト共通の描画処理
    /// @param worldTransform ワールド変換データ
    void DrawCommon(WorldTransform &worldTransform);

    //==================================================
    // メンバ変数
    //==================================================

    /// @brief レンダラーへのポインタ
    Renderer *renderer_ = nullptr;

    /// @brief マテリアル用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    /// @brief TransformationMatrix用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    /// @brief メッシュ
    std::unique_ptr<Mesh<VertexData>> mesh_;
    /// @brief 頂点数
    UINT vertexCount_ = 0;
    /// @brief インデックス数
    UINT indexCount_ = 0;

    /// @brief マテリアルマップ
    Material *materialMap_ = nullptr;
    /// @brief TransformationMatrixマップ
    TransformationMatrix *transformationMatrixMap_ = nullptr;

    /// @brief 変形用のtransform
    Transform transform_ = {
        { 1.0f, 1.0f, 1.0f },   // スケール
        { 0.0f, 0.0f, 0.0f },   // 回転
        { 0.0f, 0.0f, 0.0f }    // 平行移動
    };
    /// @brief UV用のtransform
    Transform uvTransform_ = {
        { 1.0f, 1.0f, 1.0f },   // スケール
        { 0.0f, 0.0f, 0.0f },   // 回転
        { 0.0f, 0.0f, 0.0f }    // 平行移動
    };
    /// @brief ワールド行列
    Matrix4x4 worldMatrix_{};
    /// @brief マテリアルデータ
    Material material_;

    /// @brief 使用するテクスチャのインデックス
    int useTextureIndex_ = -1;
    /// @brief 法線のタイプ
    NormalType normalType_ = kNormalTypeVertex;
    /// @brief 塗りつぶしモード
    FillMode fillMode_ = kFillModeSolid;

    /// @brief カメラ使用フラグ
    bool isUseCamera_ = false;
};

} // namespace KashipanEngine