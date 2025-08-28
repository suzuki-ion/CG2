#pragma once
#include "WorldTransform.h"
#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "Common/TransformationMatrix.h"
#include "Common/Material.h"
#include "3d/PrimitiveDrawer.h"

class Engine;
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
        std::string *pipeLineName = nullptr;
    };

    static void Initialize(Engine *engine);
    Object() noexcept;
    virtual ~Object() noexcept = default;

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
    Object(Object &&other) noexcept;
    Object &operator=(Object &&) noexcept = delete;

    /// @brief オブジェクトの名前設定
    /// @param name オブジェクトの名前
    void SetName(const std::string &name) {
        name_ = name;
    }

    /// @brief 使用するレンダリングパイプライン名の設定
    /// @param pipelineName レンダリングパイプライン名
    void SetPipelineName(const std::string &pipelineName) {
        pipeLineName_ = pipelineName;
    }

    /// @brief オブジェクトの名前の取得
    /// @return オブジェクトの名前
    const std::string &GetName() {
        return name_;
    }

    /// @brief 頂点数の取得
    /// @return 頂点数
    UINT GetVertexCount() const {
        return vertexCount_;
    }
    /// @brief インデックス数の取得
    /// @return インデックス数
    UINT GetIndexCount() const {
        return indexCount_;
    }

    /// @brief 描画フラグの取得
    /// @return 描画フラグ
    bool IsDraw() const {
        return isDraw_;
    }

    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    virtual void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }

    /// @brief オブジェクト情報へのポインタを取得
    /// @return オブジェクト情報へのポインタ
    [[nodiscard]] virtual StatePtr GetStatePtr() {
        return { nullptr, &transform_, &uvTransform_, &material_, &useTextureIndex_, &normalType_, &pipeLineName_};
    }

    /// @brief オブジェクトの描画処理
    virtual void Draw() {
        DrawCommon();
    }

    /// @brief オブジェクトの描画処理
    /// @param worldTransform ワールド変換データ
    virtual void Draw(WorldTransform &worldTransform) {
        DrawCommon(worldTransform);
    }

    /// @brief 描画フラグの設定(表示)
    void Show() {
        isDraw_ = true;
    }

    /// @brief 描画フラグの設定(非表示)
    void Hide() {
        isDraw_ = false;
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
    /// @param color 色
    void DrawCommon(WorldTransform &worldTransform);

    //==================================================
    // メンバ変数
    //==================================================

    /// @brief オブジェクトの名前
    std::string name_;
    /// @brief 使用するレンダリングパイプライン名
    std::string pipeLineName_ = "Object3d.Solid.BlendNormal";

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

    /// @brief カメラ使用フラグ
    bool isUseCamera_ = false;
    /// @brief 描画フラグ
    bool isDraw_ = true;
};

} // namespace KashipanEngine