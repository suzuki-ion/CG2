#pragma once
#include <array>
#include <vector>
#include <memory>

#include "Common/PipeLineSet.h"
#include "3d/PrimitiveDrawer.h"
#include "Math/Matrix4x4.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;
class ImGuiManager;
class TextureManager;
class ShadowMap;

struct DirectionalLight;
struct Object;
struct Triangle;
struct Sprite;
struct Sphere;
struct BillBoard;
struct ModelData;
struct Tetrahedron;
struct Plane;

enum DrawType {
    kDrawTypeNormal,
    kDrawTypeShadowMap,
};

/// @brief 描画用クラス
class Drawer {
public:
    /// @brief コンストラクタ
    /// @param winApp WinAppインスタンス
    /// @param dxCommon DirectXCommonインスタンス
    /// @param primitiveDrawer PrimitiveDrawerインスタンス
    /// @param imguiManager ImGuiManagerインスタンス
    Drawer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, TextureManager *textureManager);

    /// @brief デストラクタ
    ~Drawer();

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

    /// @brief シャドウマップの設定
    /// @param shadowMap シャドウマップへのポインタ
    void SetShadowMap(ShadowMap *shadowMap) {
        shadowMap_ = shadowMap;
    }

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

    /// @brief 描画するオブジェクトの設定
    /// @param object 描画するオブジェクトへのポインタ
    void DrawSet(Object *object);

private:
    /// @brief 平行光源の設定
    /// @param light 平行光源へのポインタ
    void SetLightBuffer(DirectionalLight *light);

    /// @brief 三角形を描画する
    /// @param triangle 描画する三角形へのポインタ
    void Draw(Triangle *triangle);

    /// @brief スプライトを描画する
    /// @param sprite 描画するスプライトへのポインタ
    void Draw(Sprite *sprite);

    /// @brief 球体を描画する
    /// @param sphere 描画する球体へのポインタ
    void Draw(Sphere *sphere);

    /// @brief ビルボードを描画する
    /// @param billboard 描画するビルボードへのポインタ
    void Draw(BillBoard *billboard);

    /// @brief モデルを描画する
    /// @param model 描画するモデルへのポインタ
    void Draw(ModelData *model);

    /// @brief 正四面体を描画する
    /// @param tetrahedron 描画する正四面体へのポインタ
    void Draw(Tetrahedron *tetrahedron);

    /// @brief 平面を描画する
    /// @param plane 描画する平面へのポインタ
    void Draw(Plane *plane);

    /// @brief 共通の描画処理
    void DrawCommon(std::vector<Object *> &objects);

    /// @brief 共通の描画処理
    void DrawCommon(Object *object);

    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;
    /// @brief ImGuiManagerインスタンス
    ImGuiManager *imguiManager_ = nullptr;
    /// @brief TextureManagerインスタンス
    TextureManager *textureManager_ = nullptr;
    /// @brief ShadowMapインスタンス
    ShadowMap *shadowMap_ = nullptr;

    /// @brief ブレンドモード
    BlendMode blendMode_ = kBlendModeNormal;
    /// @brief パイプラインセット
    std::array<std::array<PipeLineSet, kBlendModeMax>, 2> pipelineSet_;

    /// @brief デバッグカメラ使用フラグ
    bool isUseDebugCamera_ = false;
    /// @brief 平行光源へのポインタ
    DirectionalLight *directionalLight_ = nullptr;
    /// @brief 描画するオブジェクト
    std::vector<Object *> drawObjects_;
    /// @brief 描画する半透明オブジェクト
    std::vector<Object *> drawAlphaObjects_;
    /// @brief 描画する2Dオブジェクト
    std::vector<Object *> draw2DObjects_;

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