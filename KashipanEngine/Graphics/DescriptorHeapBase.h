#pragma once
#include <d3dx12.h>
#include <wrl.h>
#include <cassert>

namespace KashipanEngine {

struct CPUDescriptorHandleInfo {
    CPUDescriptorHandleInfo(const D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle, UINT handleIndex)
        : handle(cpuHandle), index(handleIndex) {
    }
    const D3D12_CPU_DESCRIPTOR_HANDLE handle;
    const UINT index;
};

struct GPUDescriptorHandleInfo {
    GPUDescriptorHandleInfo(const D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle, UINT handleIndex)
        : handle(gpuHandle), index(handleIndex) {
    }
    const D3D12_GPU_DESCRIPTOR_HANDLE handle;
    const UINT index;
};

/// @brief ディスクリプタヒープ基底クラス
class DescriptorHeapBase {
public:
    DescriptorHeapBase(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
        : heapType_(type), numDescriptors_(numDescriptors) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = type;
        desc.NumDescriptors = numDescriptors;
        desc.Flags = flags;
        desc.NodeMask = 0;
        device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap_));
        descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);
        isShaderVisible_ = (flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) != 0;
    }
    virtual ~DescriptorHeapBase() = default;

    /// @brief 指定のインデックスのCPUハンドルを取得
    /// @param index 取得したいハンドルのインデックス
    /// @return CPUハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
    /// @brief 指定のインデックスのGPUハンドルを取得
    /// @param index 取得したいハンドルのインデックス
    /// @return GPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index);

    /// @brief CPUハンドル情報を取得(取得後は次のハンドルを指す)
    /// @return CPUハンドル情報
    CPUDescriptorHandleInfo GetCPUHandleInfo();
    /// @brief GPUハンドル情報を取得(取得後は次のハンドルを指す)
    /// @return GPUハンドル情報
    GPUDescriptorHandleInfo GetGPUHandleInfo();

    /// @brief ディスクリプタの数を取得
    /// @return ディスクリプタの数
    UINT GetNumDescriptors() const { return numDescriptors_; }

    /// @brief 現在のCPUハンドルのインデックスを取得
    /// @return CPUハンドルのインデックス
    UINT GetCurrentCPUHandleIndex() const { return cpuHandleIndex; }
    /// @brief 現在のGPUハンドルのインデックスを取得
    /// @return GPUハンドルのインデックス
    UINT GetCurrentGPUHandleIndex() const { return gpuHandleIndex; }
    
protected:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
    UINT descriptorSize_;
    UINT numDescriptors_;
    UINT cpuHandleIndex = 0;
    UINT gpuHandleIndex = 0;
    bool isShaderVisible_ = false;
    D3D12_DESCRIPTOR_HEAP_TYPE heapType_;
};

} // namespace KashipanEngine