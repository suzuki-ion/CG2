#include <cassert>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "3d/PrimitiveDrawer.h"
#include "Common/Logs.h"
#include "Common/Descriptors/RTV.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"

#include "ScreenBuffer.h"

namespace KashipanEngine {

namespace {

// 初期化フラグ
bool isInitialized = false;
// WinApp
WinApp *sWinApp = nullptr;
// DirectXCommonクラスへのポインタ
DirectXCommon *sDxCommon = nullptr;
/// @brief コマンドリストへのポインタ
ID3D12GraphicsCommandList *sCommandList = nullptr;
/// @brief コマンドキューへのポインタ
ID3D12CommandQueue *sCommandQueue = nullptr;
/// @brief デバイスへのポインタ
ID3D12Device *sDevice = nullptr;

} // namespace

void ScreenBuffer::Initialize(WinApp *winApp, DirectXCommon *dxCommon) {
    sWinApp = winApp;
    sDxCommon = dxCommon;
    sCommandList = sDxCommon->GetCommandList();
    sCommandQueue = sDxCommon->GetCommandQueue();
    sDevice = sDxCommon->GetDevice();
    isInitialized = true;
}

ScreenBuffer::ScreenBuffer(uint32_t width, uint32_t height) {
    assert(isInitialized);

    screenWidth_ = width;
    screenHeight_ = height;
    pipelineSet_ = PrimitiveDrawer::CreateGraphicsPipeline(
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeNormal, true);
    CreateTextureResource();
    CreateDepthStencilResource();
    CreateRenderTarget();
    CreateShaderResource();
    CreateDepthStencil();
}

void ScreenBuffer::PreDraw() {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = endState;
    barrier.Transition.StateAfter = beginState;
    sDxCommon->SetBarrier(barrier);

    // 画面クリア
    sCommandList->OMSetRenderTargets(1, &rtvCPUHandle_, false, &dsvCPUHandle_);
    sCommandList->ClearRenderTargetView(rtvCPUHandle_, clearValue_.Color, 0, nullptr);
    sCommandList->ClearDepthStencilView(dsvCPUHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


}

void ScreenBuffer::PostDraw() {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = beginState;
    barrier.Transition.StateAfter = endState;
    sDxCommon->SetBarrier(barrier);
}

void ScreenBuffer::CreateTextureResource() {
    //==================================================
    // Resourceの設定
    //==================================================

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = screenWidth_;
    resourceDesc.Height = screenHeight_;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    //==================================================
    // 利用するHeapの設定
    //==================================================

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //==================================================
    // クリア値
    //==================================================

    clearValue_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    clearValue_.Color[0] = 0.0f;
    clearValue_.Color[1] = 0.0f;
    clearValue_.Color[2] = 0.0f;
    clearValue_.Color[3] = 1.0f;

    //==================================================
    // Resourceを生成する
    //==================================================

    HRESULT hr = sDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue_,
        IID_PPV_ARGS(resource_.GetAddressOf())
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

void ScreenBuffer::CreateDepthStencilResource() {
    //==================================================
    // Resourceの設定
    //==================================================

    D3D12_RESOURCE_DESC depthDesc = {};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Alignment = 0;
    depthDesc.Width = screenWidth_;
    depthDesc.Height = screenHeight_;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    
    //==================================================
    // 利用するHeapの設定
    //==================================================

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //==================================================
    // クリア値
    //==================================================

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    //==================================================
    // Resourceの作成
    //==================================================

    HRESULT hr = sDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(depthStencilResource_.GetAddressOf())
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

void ScreenBuffer::CreateRenderTarget() {
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    rtvCPUHandle_ = RTV::GetCPUDescriptorHandle();
    sDevice->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvCPUHandle_);
}

void ScreenBuffer::CreateShaderResource() {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvCPUHandle_ = SRV::GetCPUDescriptorHandle();
    srvGPUHandle_ = SRV::GetGPUDescriptorHandle();
    sDevice->CreateShaderResourceView(resource_.Get(), &srvDesc, srvCPUHandle_);
}

void ScreenBuffer::CreateDepthStencil() {
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvCPUHandle_ = DSV::GetCPUDescriptorHandle();
    sDevice->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvCPUHandle_);
}

} // namespace KashipanEngine