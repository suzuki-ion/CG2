#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include "Common/PipeLineSet.h"

namespace KashipanEngine {

class WinApp;
class DirectXCommon;
class Renderer;

class ScreenBuffer {
public:
    /// @brief 初期化用関数
    /// @param winApp WinAppクラスへのポインタ
    /// @param dxCommon DirectXCommonクラスへのポインタ
    static void Initialize(WinApp *winApp, DirectXCommon *dxCommon);

    /// @brief スクリーンバッファのコンストラクタ
    /// @param width スクリーンの横幅
    /// @param height スクリーンの縦幅
    ScreenBuffer(uint32_t width, uint32_t height);
    
    /// @brief レンダラーの設定
    /// @param renderer レンダラーへのポインタ
    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();

private:
    static inline const D3D12_RESOURCE_STATES beginState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    static inline const D3D12_RESOURCE_STATES endState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    void CreateTextureResource();
    void CreateDepthStencilResource();
    void CreateRenderTarget();
    void CreateShaderResource();
    void CreateDepthStencil();

    // レンダラーへのポインタ
    Renderer *renderer_ = nullptr;

    /// @brief 画面クリア時の値
    D3D12_CLEAR_VALUE clearValue_;

    /// @brief スクリーンの横幅
    uint32_t screenWidth_ = 0;
    /// @brief スクリーンの縦幅
    uint32_t screenHeight_ = 0;

    // パイプラインセット
    PipeLineSet pipelineSet_;
    
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