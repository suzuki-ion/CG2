#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Windows.h>
#include <cassert>
#include <format>

#include "DirectXCommon.h"
#include "DirectXDevice.h"
#include "SwapChain.h"
#include "RenderTargetManager.h"
#include "ResourceFactory.h"
#include "Core/WinApp/WinApp.h"
#include "Math/Vector4.h"
#include "Common/Logs.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace KashipanEngine {

DirectXCommon::DirectXCommon(bool enableDebugLayer, WinApp* winApp) : winApp_(winApp) {
    assert(winApp != nullptr);

    // 各管理クラスを順番に初期化
    device_ = std::make_unique<DirectXDevice>(enableDebugLayer);
    swapChain_ = std::make_unique<SwapChain>(device_.get(), winApp_);
    
    // WinAppからサイズを取得して渡す
    int32_t width = winApp_->GetClientWidth();
    int32_t height = winApp_->GetClientHeight();
    renderTargetManager_ = std::make_unique<RenderTargetManager>(device_.get(), swapChain_.get(), width, height);
    
    resourceFactory_ = std::make_unique<ResourceFactory>(device_.get());

    Log("DirectXCommon initialized with modular architecture.");
    LogNewLine();
}

DirectXCommon::~DirectXCommon() {
    // 逆順で破棄
    resourceFactory_.reset();
    renderTargetManager_.reset();
    swapChain_.reset();
    device_.reset();
    
    winApp_ = nullptr;

    Log("DirectXCommon finalized.");
    LogNewLine();
}

void DirectXCommon::PreDraw() {
    // バックバッファインデックス取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // レンダーターゲット用のバリア設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChain_->GetBackBuffer(backBufferIndex);
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    SetBarrier(barrier);

    // レンダーターゲット設定とクリア
    renderTargetManager_->SetRenderTarget();
    renderTargetManager_->ClearRenderTarget();
    renderTargetManager_->ClearDepthStencil();

    // ビューポートとシザー矩形設定
    renderTargetManager_->SetViewportAndScissorRect();
}

void DirectXCommon::PostDraw() {
    // バックバッファインデックス取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // Present用のバリア設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChain_->GetBackBuffer(backBufferIndex);
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    SetBarrier(barrier);

    // コマンド実行とPresent
    CommandExecute(true);
}

void DirectXCommon::ClearRenderTarget() {
    renderTargetManager_->ClearRenderTarget();
}

void DirectXCommon::ClearDepthStencil() {
    renderTargetManager_->ClearDepthStencil();
    renderTargetManager_->SetRenderTarget();
}

void DirectXCommon::SetBarrier(D3D12_RESOURCE_BARRIER& barrier) {
    // 同じバリアを張ろうとしているのは無視
    if (barrier.Transition.StateAfter == currentBarrierState_) {
        return;
    }

    // バリアを張る
    device_->GetCommandList()->ResourceBarrier(1, &barrier);
    
    // バリア状態を更新
    currentBarrierState_ = barrier.Transition.StateAfter;
}

void DirectXCommon::SetClearColor(float r, float g, float b, float a) {
    renderTargetManager_->SetClearColor(r, g, b, a);
}

void DirectXCommon::SetClearColor(const Vector4& color) {
    renderTargetManager_->SetClearColor(color);
}

void DirectXCommon::Resize() {
    // WinAppから最新のサイズを取得
    int32_t width = winApp_->GetClientWidth();
    int32_t height = winApp_->GetClientHeight();
    
    // 各コンポーネントをサイズ指定でリサイズ
    swapChain_->Resize(width, height);
    renderTargetManager_->Resize(width, height);
}

void DirectXCommon::CommandExecute(bool isSwapChain) {
    device_->ExecuteCommand();
    
    if (isSwapChain) {
        swapChain_->Present();
    }
}

void DirectXCommon::CreateDescriptorHeap(
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    UINT numDescriptors,
    bool shaderVisible
) {
    device_->CreateDescriptorHeap(descriptorHeap, heapType, numDescriptors, shaderVisible);
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(
    int32_t width,
    int32_t height,
    DXGI_FORMAT format,
    D3D12_RESOURCE_STATES initialState
) {
    return resourceFactory_->CreateDepthStencilTextureResource(width, height, format, initialState);
}

// 後方互換性のためのアクセサ実装
ID3D12Device* DirectXCommon::GetDevice() const {
    return device_->GetDevice();
}

ID3D12CommandQueue* DirectXCommon::GetCommandQueue() const {
    return device_->GetCommandQueue();
}

ID3D12GraphicsCommandList* DirectXCommon::GetCommandList() const {
    return device_->GetCommandList();
}

DXGI_SWAP_CHAIN_DESC1 DirectXCommon::GetSwapChainDesc() const {
    return swapChain_->GetSwapChainDesc();
}

D3D12_RENDER_TARGET_VIEW_DESC DirectXCommon::GetRTVDesc() const {
    return renderTargetManager_->GetRTVDesc();
}

} // namespace KashipanEngine