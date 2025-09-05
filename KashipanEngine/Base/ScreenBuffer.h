#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <string>

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

    /// @brief スクリーンバッファの取得
    /// @param screenName スクリーンの名前
    /// @return スクリーンバッファへのポインタ
    static ScreenBuffer *GetScreenBuffer(const std::string &screenName);

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

    /// @brief SRVのCPUハンドルを取得
    /// @return 
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const {
        return srvGPUHandle_;
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
};

} // namespace KashipanEngine