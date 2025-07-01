#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace KashipanEngine {

struct TextureData {
    /// @brief テクスチャの名前
    std::string name;
    /// @brief テクスチャが読み込まれたインデックス
    uint32_t index = -1;
    /// @brief テクスチャリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    /// @brief 中間リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
    /// @brief SRVハンドル(CPU)
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
    /// @brief SRVハンドル(GPU)
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
    /// @brief テクスチャの幅
    uint32_t width;
    /// @brief テクスチャの高さ
    uint32_t height;
};

} // namespace KashipanEngine