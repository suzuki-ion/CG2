#include "SwapChain.h"
#include "DirectXDevice.h"
#include "Common/Logs.h"
#include <cassert>

namespace KashipanEngine {

SwapChain::SwapChain(DirectXDevice* device, int32_t width, int32_t height, HWND hwnd) : device_(device) {
    assert(device != nullptr);

    currentWidth_ = width;
    currentHeight_ = height;
    InitializeSwapChain();
    InitializeBackBuffers();

    Log("SwapChain initialized.");
}

SwapChain::~SwapChain() {
    Log("SwapChain finalized.");
}

void SwapChain::Present() {
    swapChain_->Present(1, 0);
}

void SwapChain::Resize(int32_t width, int32_t height) {
    // サイズが変わっていない場合は何もしない
    if (currentWidth_ == width && currentHeight_ == height) {
        return;
    }

    currentWidth_ = width;
    currentHeight_ = height;

    // バックバッファをクリア
    for (UINT i = 0; i < BUFFER_COUNT; ++i) {
        backBuffers_[i] = nullptr;
    }

    // スワップチェインのサイズを変更
    HRESULT hr = swapChain_->ResizeBuffers(
        BUFFER_COUNT,
        static_cast<UINT>(width),
        static_cast<UINT>(height),
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    );
    assert(SUCCEEDED(hr));

    // スワップチェイン設定も更新
    swapChainDesc_.Width = static_cast<UINT>(width);
    swapChainDesc_.Height = static_cast<UINT>(height);

    // バックバッファを再初期化
    InitializeBackBuffers();

    Log("SwapChain resized.");
}

UINT SwapChain::GetCurrentBackBufferIndex() const {
    return swapChain_->GetCurrentBackBufferIndex();
}

ID3D12Resource* SwapChain::GetBackBuffer(UINT index) const {
    assert(index < BUFFER_COUNT);
    return backBuffers_[index].Get();
}

void SwapChain::InitializeSwapChain() {
    swapChainDesc_.Width = static_cast<UINT>(currentWidth_);
    swapChainDesc_.Height = static_cast<UINT>(currentHeight_);
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.BufferCount = BUFFER_COUNT;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = device_->GetDXGIFactory()->CreateSwapChainForHwnd(
        device_->GetCommandQueue(),
        winApp_->GetWindowHandle(),
        &swapChainDesc_,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())
    );
    assert(SUCCEEDED(hr));
}

void SwapChain::InitializeBackBuffers() {
    for (UINT i = 0; i < BUFFER_COUNT; ++i) {
        HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
        assert(SUCCEEDED(hr));
    }
}

} // namespace KashipanEngine