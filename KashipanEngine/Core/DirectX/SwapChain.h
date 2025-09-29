#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

namespace KashipanEngine {

// 前方宣言
class DirectXDevice;

/// @brief スワップチェイン管理クラス
/// @details スワップチェイン、バックバッファ、Present処理を管理
class SwapChain final {
public:
    SwapChain(DirectXDevice *device, int32_t width, int32_t height, HWND hwnd);
    ~SwapChain();

    // コピー・ムーブ禁止
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain(SwapChain&&) = delete;
    SwapChain& operator=(SwapChain&&) = delete;

    /// @brief Present実行
    void Present();

    /// @brief ウィンドウサイズ変更時の処理
    /// @param width 新しい幅
    /// @param height 新しい高さ
    void Resize(int32_t width, int32_t height);

    /// @brief 現在のバックバッファインデックス取得
    /// @return バックバッファインデックス
    UINT GetCurrentBackBufferIndex() const;

    /// @brief バックバッファリソース取得
    /// @param index バックバッファインデックス
    /// @return バックバッファリソース
    ID3D12Resource* GetBackBuffer(UINT index) const;

    /// @brief スワップチェイン設定取得
    /// @return スワップチェイン設定
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }

    /// @brief バックバッファの数
    static constexpr UINT BUFFER_COUNT = 2;

private:
    /// @brief スワップチェイン初期化
    void InitializeSwapChain();
    /// @brief バックバッファリソース初期化
    void InitializeBackBuffers();

private:
    DirectXDevice* device_;

    // ウィンドウハンドル
    HWND hwnd_;

    // サイズ情報（キャッシュ用）
    int32_t currentWidth_;
    int32_t currentHeight_;

    // スワップチェイン情報
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers_[BUFFER_COUNT];
};

} // namespace KashipanEngine