#pragma once
#include <wrl.h>
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Math/Vector4.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXDevice;
class SwapChain;
class RenderTargetManager;
class ResourceFactory;

/// @brief DirectX統合管理クラス
/// @details 各種DirectX関連クラスを統合して管理し、従来のインターフェースを提供
class DirectXCommon final {
public:
    DirectXCommon(bool enableDebugLayer, WinApp* winApp);
    ~DirectXCommon();

    // コピー・ムーブ禁止
    DirectXCommon(const DirectXCommon&) = delete;
    DirectXCommon& operator=(const DirectXCommon&) = delete;
    DirectXCommon(DirectXCommon&&) = delete;
    DirectXCommon& operator=(DirectXCommon&&) = delete;

    /// @brief 描画前処理
    void PreDraw();
    
    /// @brief 描画後処理
    void PostDraw();

    /// @brief レンダーターゲットのクリア
    void ClearRenderTarget();
    
    /// @brief 深度バッファのクリア
    void ClearDepthStencil();

    /// @brief バリアを張る
    /// @param barrier バリアの設定
    void SetBarrier(D3D12_RESOURCE_BARRIER& barrier);

    /// @brief レンダーターゲットのクリア色を設定
    /// @param r 赤
    /// @param g 緑
    /// @param b 青
    /// @param a 透明度
    void SetClearColor(float r, float g, float b, float a);

    /// @brief レンダーターゲットのクリア色を設定
    /// @param color RGBA
    void SetClearColor(const Vector4& color);

    /// @brief ウィンドウサイズ変更適応
    void Resize();

    /// @brief コマンドの実行
    /// @param isSwapChain スワップチェインの実行かどうか
    void CommandExecute(bool isSwapChain);

    /// @brief ディスクリプタヒープの作成
    /// @param descriptorHeap ディスクリプタヒープへのポインタ
    /// @param heapType ヒープの種類
    /// @param numDescriptors ディスクリプタの数
    /// @param shaderVisible シェーダーから見えるかどうか
    void CreateDescriptorHeap(
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible = false
    );

    /// @brief 深度バッファのリソースを作成
    /// @param width 深度バッファの幅
    /// @param height 深度バッファの高さ
    /// @param format フォーマット
    /// @param initialState 初期状態
    /// @return 深度バッファのリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
        int32_t width,
        int32_t height,
        DXGI_FORMAT format,
        D3D12_RESOURCE_STATES initialState
    );

    // アクセサ（後方互換性のため）
    ID3D12Device* GetDevice() const;
    ID3D12CommandQueue* GetCommandQueue() const;
    ID3D12GraphicsCommandList* GetCommandList() const;
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const;
    D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const;
    D3D12_RESOURCE_STATES GetCurrentBarrierState() const { return currentBarrierState_; }

    // 新しいアクセサ（内部クラスへのアクセス）
    DirectXDevice* GetDirectXDevice() const { return device_.get(); }
    SwapChain* GetSwapChain() const { return swapChain_.get(); }
    RenderTargetManager* GetRenderTargetManager() const { return renderTargetManager_.get(); }
    ResourceFactory* GetResourceFactory() const { return resourceFactory_.get(); }

private:
    WinApp* winApp_;
    
    // 各機能の管理クラス
    std::unique_ptr<DirectXDevice> device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::unique_ptr<RenderTargetManager> renderTargetManager_;
    std::unique_ptr<ResourceFactory> resourceFactory_;

    // バリア状態管理（従来の互換性のため）
    D3D12_RESOURCE_STATES currentBarrierState_ = D3D12_RESOURCE_STATE_COMMON;
};

} // namespace KashipanEngine