#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace KashipanEngine {

struct ScreenBuffer {
    void Initialize(
        ID3D12Device *device,
        ID3D12DescriptorHeap *rtvHeap,
        ID3D12DescriptorHeap *dsvHeap,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format
    );

    /// @brief 描画用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    /// @brief 深度バッファ用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> depthResource_;
    /// @brief 描画に使用するRTVハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
    /// @brief 描画に使用するDSVハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_;
};

} // namespace KashipanEngine