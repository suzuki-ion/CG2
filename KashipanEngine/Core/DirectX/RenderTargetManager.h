#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

namespace KashipanEngine {

// 前方宣言
class DirectXDevice;
class SwapChain;
struct Vector4;

/// @brief レンダーターゲット管理クラス
/// @details RTV、DSV、ビューポート、シザー矩形、クリア処理を管理
class RenderTargetManager final {
public:
    RenderTargetManager(DirectXDevice* device, SwapChain* swapChain, int32_t width, int32_t height);
    ~RenderTargetManager();

    // コピー・ムーブ禁止
    RenderTargetManager(const RenderTargetManager&) = delete;
    RenderTargetManager& operator=(const RenderTargetManager&) = delete;
    RenderTargetManager(RenderTargetManager&&) = delete;
    RenderTargetManager& operator=(RenderTargetManager&&) = delete;

    /// @brief 描画ターゲットを設定
    void SetRenderTarget();

    /// @brief レンダーターゲットクリア
    void ClearRenderTarget();

    /// @brief 深度バッファクリア
    void ClearDepthStencil();

    /// @brief ビューポートとシザー矩形を設定
    void SetViewportAndScissorRect();

    /// @brief リサイズ処理
    /// @param width 新しい幅
    /// @param height 新しい高さ
    void Resize(int32_t width, int32_t height);

    /// @brief クリア色設定
    /// @param r 赤
    /// @param g 緑
    /// @param b 青
    /// @param a 透明度
    void SetClearColor(float r, float g, float b, float a);

    /// @brief クリア色設定
    /// @param color RGBA
    void SetClearColor(const Vector4& color);

    /// @brief RTV設定取得
    /// @return RTV設定
    D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }

    /// @brief DSVハンドル取得
    /// @return DSVハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHandle_; }

    /// @brief 現在の幅を取得
    /// @return 幅
    int32_t GetWidth() const { return width_; }

    /// @brief 現在の高さを取得
    /// @return 高さ
    int32_t GetHeight() const { return height_; }

private:
    /// @brief RTV初期化
    void InitializeRTV();

    /// @brief DSV初期化
    void InitializeDSV();

    /// @brief ビューポートとシザー矩形初期化
    void InitializeViewportAndScissorRect();

    /// @brief RTVハンドル初期化
    void InitializeRTVHandles();

    /// @brief DSVハンドル初期化
    void InitializeDSVHandle();

private:
    DirectXDevice* device_;
    SwapChain* swapChain_;

    // サイズ情報（WinAppの代わり）
    int32_t width_;
    int32_t height_;

    // レンダーターゲット関連
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

    // 深度バッファ関連
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

    // ビューポート・シザー矩形
    D3D12_VIEWPORT viewport_;
    D3D12_RECT scissorRect_;

    // クリア色
    float clearColor_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

} // namespace KashipanEngine