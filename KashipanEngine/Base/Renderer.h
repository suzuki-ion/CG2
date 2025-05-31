#pragma once
#include <array>
#include <vector>
#include <memory>

#include "Common/PipeLineSet.h"
#include "Common/TransformationMatrix.h"
#include "3d/PrimitiveDrawer.h"
#include "Math/Matrix4x4.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;
class ImGuiManager;
class TextureManager;
class Camera;

struct DirectionalLight;

enum DrawType {
    kDrawTypeNormal,
    kDrawTypeShadowMap,
};

/// @brief 描画用クラス
class Renderer {
public:
    /// @brief オブジェクト情報
    struct ObjectState {
        /// @brief メッシュへのポインタ
        Mesh *mesh = nullptr;
        /// @brief マテリアル用のリソースへのポインタ
        ID3D12Resource *materialResource = nullptr;
        /// @brief transformationMatrix用のリソースへのポインタ
        ID3D12Resource *transformationMatrixResource = nullptr;
        /// @brief transformationMatrixマップ
        TransformationMatrix *transformationMatrixMap = nullptr;
        /// @brief ワールド行列
        Matrix4x4 *worldMatrix = nullptr;

        /// @brief 頂点数
        UINT vertexCount = 0;
        /// @brief インデックス数
        UINT indexCount = 0;
        /// @brief テクスチャのインデックス
        int useTextureIndex = -1;
        /// @brief 塗りつぶしモード
        FillMode fillMode = kFillModeSolid;
        /// @brief カメラを使用するかどうか
        bool isUseCamera = false;
    };

    /// @brief コンストラクタ
    /// @param winApp WinAppインスタンス
    /// @param dxCommon DirectXCommonインスタンス
    /// @param primitiveRenderer PrimitiveDrawerインスタンス
    /// @param imguiManager ImGuiManagerインスタンス
    Renderer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, TextureManager *textureManager);

    /// @brief デストラクタ
    ~Renderer();

    /// @brief 描画前処理
    void PreDraw();

    /// @brief 描画後処理
    void PostDraw();

    /// @brief デバッグカメラの切り替え
    void ToggleDebugCamera();

    /// @brief デバッグカメラが有効かどうか
    /// @return 有効ならtrue、無効ならfalse
    bool IsUseDebugCamera() const {
        return isUseDebugCamera_;
    }

    /// @brief カメラの設定
    /// @param camera カメラへのポインタ
    void SetCamera(Camera *camera);

    /// @brief ブレンドモードの設定
    /// @param blendMode ブレンドモード
    void SetBlendMode(BlendMode blendMode) {
        blendMode_ = blendMode;
    }

    /// @brief 平行光源の設定
    /// @param light 平行光源へのポインタ
    void SetLight(DirectionalLight *light) {
        directionalLight_ = light;
    }

    /// @brief 描画するオブジェクト情報の設定
    /// @param object 描画するオブジェクト情報へのポインタ
    /// @brief isUseCamera カメラを使用しているかどうか
    /// @param isSemitransparent 半透明オブジェクトかどうか
    void DrawSet(ObjectState objectState, bool isUseCamera, bool isSemitransparent);

private:
    /// @brief 平行光源の設定
    /// @param light 平行光源へのポインタ
    void SetLightBuffer(DirectionalLight *light);

    /// @brief 共通の描画処理
    void DrawCommon(std::vector<ObjectState> &objectStates);

    /// @brief 共通の描画処理
    void DrawCommon(ObjectState *objectState);

    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;
    /// @brief ImGuiManagerインスタンス
    ImGuiManager *imguiManager_ = nullptr;
    /// @brief TextureManagerインスタンス
    TextureManager *textureManager_ = nullptr;

    /// @brief ブレンドモード
    BlendMode blendMode_ = kBlendModeNormal;
    /// @brief パイプラインセット
    std::array<std::array<PipeLineSet, kBlendModeMax>, 2> pipelineSet_;

    /// @brief デバッグカメラ使用フラグ
    bool isUseDebugCamera_ = false;
    /// @brief 平行光源へのポインタ
    DirectionalLight *directionalLight_ = nullptr;
    /// @brief 描画するオブジェクト
    std::vector<ObjectState> drawObjects_;
    /// @brief 描画する半透明オブジェクト
    std::vector<ObjectState> drawAlphaObjects_;
    /// @brief 描画する2Dオブジェクト
    std::vector<ObjectState> draw2DObjects_;

    /// @brief 2D描画用のビュー行列
    Matrix4x4 viewMatrix2D_;
    /// @brief 2D描画用のプロジェクション行列
    Matrix4x4 projectionMatrix2D_;
    /// @brief 2D描画用のWVP行列
    Matrix4x4 wvpMatrix2D_;

    /// @brief ビューポートの設定
    D3D12_VIEWPORT viewport_ = {};
    /// @brief シザー矩形の設定
    D3D12_RECT scissorRect_ = {};
};

} // namespace KashipanEngine