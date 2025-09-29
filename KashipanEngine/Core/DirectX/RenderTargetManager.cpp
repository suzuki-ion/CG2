#include "RenderTargetManager.h"
#include "DirectXDevice.h"
#include "SwapChain.h"
#include "Math/Vector4.h"
#include "Common/Descriptors/RTV.h"
#include "Common/Descriptors/DSV.h"
#include "Common/Logs.h"
#include <cassert>

namespace KashipanEngine {

RenderTargetManager::RenderTargetManager(DirectXDevice* device, SwapChain* swapChain, int32_t width, int32_t height)
    : device_(device), swapChain_(swapChain), width_(width), height_(height) {
    assert(device != nullptr);
    assert(swapChain != nullptr);
    assert(width > 0);
    assert(height > 0);

    InitializeViewportAndScissorRect();
    InitializeRTV();
    InitializeDSV();
    InitializeRTVHandles();
    InitializeDSVHandle();

    Log("RenderTargetManager initialized.");
}

RenderTargetManager::~RenderTargetManager() {
    Log("RenderTargetManager finalized.");
}

void RenderTargetManager::SetRenderTarget() {
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
    device_->GetCommandList()->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle_);
}

void RenderTargetManager::ClearRenderTarget() {
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
    device_->GetCommandList()->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor_, 0, nullptr);
}

void RenderTargetManager::ClearDepthStencil() {
    device_->GetCommandList()->ClearDepthStencilView(
        dsvHandle_,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr
    );
}

void RenderTargetManager::SetViewportAndScissorRect() {
    device_->GetCommandList()->RSSetViewports(1, &viewport_);
    device_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

void RenderTargetManager::Resize(int32_t width, int32_t height) {
    width_ = width;
    height_ = height;
    
    InitializeViewportAndScissorRect();
    
    // DSVの再初期化
    InitializeDSVHandle();
    
    // RTVハンドルの再初期化
    InitializeRTVHandles();

    Log("RenderTargetManager resized.");
}

void RenderTargetManager::SetClearColor(float r, float g, float b, float a) {
    clearColor_[0] = r;
    clearColor_[1] = g;
    clearColor_[2] = b;
    clearColor_[3] = a;
}

void RenderTargetManager::SetClearColor(const Vector4& color) {
    clearColor_[0] = color.x;
    clearColor_[1] = color.y;
    clearColor_[2] = color.z;
    clearColor_[3] = color.w;
}

void RenderTargetManager::InitializeRTV() {
    RTV::Initialize(device_);
    Log("RTV initialized.");
}

void RenderTargetManager::InitializeDSV() {
    DSV::Initialize(width_, height_, device_);
    Log("DSV initialized.");
}

void RenderTargetManager::InitializeViewportAndScissorRect() {
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.Width = static_cast<float>(width_);
    viewport_.Height = static_cast<float>(height_);
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;

    scissorRect_.left = 0;
    scissorRect_.top = 0;
    scissorRect_.right = static_cast<LONG>(width_);
    scissorRect_.bottom = static_cast<LONG>(height_);
}

void RenderTargetManager::InitializeRTVHandles() {
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // RTVハンドルを取得してRTVを作成
    for (UINT i = 0; i < SwapChain::BUFFER_COUNT; ++i) {
        rtvHandles_[i] = RTV::GetCPUDescriptorHandle();
        device_->GetDevice()->CreateRenderTargetView(
            swapChain_->GetBackBuffer(i),
            &rtvDesc_,
            rtvHandles_[i]
        );
    }

    Log("RTV handles initialized.");
}

void RenderTargetManager::InitializeDSVHandle() {
    dsvHandle_ = DSV::GetCPUDescriptorHandle(0);
    Log("DSV handle initialized.");
}

} // namespace KashipanEngine