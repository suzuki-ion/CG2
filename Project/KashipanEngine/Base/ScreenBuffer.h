#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <string>
#include "Math/Vector2.h"

namespace KashipanEngine {

class WinApp;
class DirectXCommon;
class Renderer;
class Camera;
class PipeLineManager;

class ScreenBuffer {
public:
    /// @brief 初期化用関数
    /// @param winApp WinAppクラスへのポインタ
    /// @param dxCommon DirectXCommonクラスへのポインタ
    /// @param pipeLineManager PipeLineManagerクラスへのポインタ
    static void Initialize(WinApp *winApp, DirectXCommon *dxCommon, PipeLineManager *pipeLineManager);

    /// @brief スクリーンバッファのコンストラクタ
    /// @param screenName スクリーンの名前
    /// @param width スクリーンの横幅
    /// @param height スクリーンの縦幅
    ScreenBuffer(const std::string screenName, uint32_t width, uint32_t height);
    
    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }

    /// @brief サイズ変更
    /// @param width スクリーンの横幅
    /// @param height スクリーンの縦幅
    void Resize(uint32_t width, uint32_t height);

    /// @brief スクリーンの名前を取得
    /// @return スクリーンの名前
    const std::string &GetScreenName() const {
        return screenName_;
    }

    /// @brief スクリーンのテクスチャのインデックスを取得
    /// @return スクリーンのテクスチャのインデックス
    uint32_t GetTextureIndex() const;

    /// @brief 現在のスケールを取得
    /// @return 現在のスケール
    Vector2 GetCurrentScale() const {
        return currentScale_;
    }
    /// @brief 現在の左上座標を取得
    /// @return 現在の左上座標
    Vector2 GetCurrentLeftTopPos() const {
        return currentLeftTopPos_;
    }

    /// @brief デバッグ用：画像の描画領域情報を取得
    /// @param outImageSize 画像の実際の描画サイズ
    /// @param outWindowPos ImGuiウィンドウの位置
    /// @param outContentMin コンテンツ領域の最小座標
    void GetDebugInfo(Vector2& outImageSize, Vector2& outWindowPos, Vector2& outContentMin) const {
#if !RELEASE_BUILD
        outImageSize.x = static_cast<float>(screenWidth_) * currentScale_.x;
        outImageSize.y = static_cast<float>(screenHeight_) * currentScale_.y;
        outWindowPos = currentLeftTopPos_;
        outContentMin = currentLeftTopPos_;
#else
        outImageSize = Vector2(0.0f, 0.0f);
        outWindowPos = Vector2(0.0f, 0.0f);
        outContentMin = Vector2(0.0f, 0.0f);
#endif
    }

    /// @brief スクリーン座標をバッファ座標に変換
    /// @param screenX スクリーン上のX座標
    /// @param screenY スクリーン上のY座標
    /// @param outBufferX バッファ上のX座標（出力）
    /// @param outBufferY バッファ上のY座標（出力）
    /// @return 変換が成功したかどうか
    bool ScreenToBuffer(int screenX, int screenY, int& outBufferX, int& outBufferY) const {
#if !RELEASE_BUILD
        // スクリーン座標がバッファの描画領域内にあるかチェック
        float relativeX = screenX - currentLeftTopPos_.x;
        float relativeY = screenY - currentLeftTopPos_.y;
        
        float imageWidth = static_cast<float>(screenWidth_) * currentScale_.x;
        float imageHeight = static_cast<float>(screenHeight_) * currentScale_.y;
        
        if (relativeX < 0 || relativeY < 0 || relativeX >= imageWidth || relativeY >= imageHeight) {
            return false; // 範囲外
        }
        
        // バッファ座標に変換
        outBufferX = static_cast<int>(relativeX / currentScale_.x);
        outBufferY = static_cast<int>(relativeY / currentScale_.y);
        return true;
#else
        outBufferX = screenX;
        outBufferY = screenY;
        return true;
#endif
    }

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();

    /// @brief ImGui上での描画
    void DrawToImGui();

private:
    static inline const D3D12_RESOURCE_STATES beginState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    static inline const D3D12_RESOURCE_STATES endState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    void CreateTextureResource();
    void CreateDepthStencilResource();
    void CreateRenderTarget();
    void CreateShaderResource();
    void CreateDepthStencil();

    // スクリーン名
    std::string screenName_ = "ScreenBuffer";

    // レンダラーへのポインタ
    Renderer *renderer_ = nullptr;
    // メインカメラへのポインタ
    Camera *mainCamera_ = nullptr;
    // デバッグ用カメラへのポインタ
    Camera *debugCamera_ = nullptr;

    /// @brief 画面クリア時の値
    D3D12_CLEAR_VALUE clearValue_;

    /// @brief スクリーンの横幅
    uint32_t screenWidth_ = 0;
    /// @brief スクリーンの縦幅
    uint32_t screenHeight_ = 0;

    /// @brief ビューポート
    D3D12_VIEWPORT viewport_ = {};
    /// @brief シザー矩形
    D3D12_RECT scissorRect_ = {};

    /// @brief スクリーンのテクスチャのインデックス
    uint32_t textureIndex_ = 0;

    /// @brief 描画用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    /// @brief 深度バッファ用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    
    /// @brief RTVのCPUハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle_;
    
    /// @brief SRVのGPUハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle_;
    /// @brief SRVのGPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle_;
    
    /// @brief DSVのCPUハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle_;

    // スクリーンの現在のスケール
    Vector2 currentScale_ = { 1.0f, 1.0f };
    // スクリーンの現在の左上座標
    Vector2 currentLeftTopPos_ = { 0.0f, 0.0f };
};

} // namespace KashipanEngine