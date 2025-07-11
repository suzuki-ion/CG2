#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace KashipanEngine {

struct ScreenBuffer {
    static inline const D3D12_RESOURCE_STATES beginState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    static inline const D3D12_RESOURCE_STATES endState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    /// @brief 描画用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    /// @brief 深度バッファ用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> depthResource;
    /// @brief 描画に使用するRTVハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    /// @brief 描画に使用するSRVハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
    /// @brief 描画に使用するDSVハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
};

} // namespace KashipanEngine