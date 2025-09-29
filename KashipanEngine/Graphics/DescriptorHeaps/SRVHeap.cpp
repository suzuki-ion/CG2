#include "SRVHeap.h"

namespace KashipanEngine {

SRVHeap::SRVHeap(ID3D12Device* device, UINT numDescriptors)
    : DescriptorHeapBase(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
}

CPUDescriptorHandleInfo SRVHeap::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc) {
    if (!resource) {
        assert(false && "Resource is null.");
        return CPUDescriptorHandleInfo({}, 0);
    }

    // CPUハンドル情報を取得
    CPUDescriptorHandleInfo handleInfo = GetCPUHandleInfo();
    
    // デバイスを取得してSRVを作成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    heap_->GetDevice(IID_PPV_ARGS(&device));
    device->CreateShaderResourceView(resource, desc, handleInfo.handle);

    return handleInfo;
}

CPUDescriptorHandleInfo SRVHeap::CreateCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc) {
    // CPUハンドル情報を取得
    CPUDescriptorHandleInfo handleInfo = GetCPUHandleInfo();
    
    // デバイスを取得してCBVを作成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    heap_->GetDevice(IID_PPV_ARGS(&device));
    device->CreateConstantBufferView(&desc, handleInfo.handle);

    return handleInfo;
}

CPUDescriptorHandleInfo SRVHeap::CreateUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc) {
    if (!resource) {
        assert(false && "Resource is null.");
        return CPUDescriptorHandleInfo({}, 0);
    }

    // CPUハンドル情報を取得
    CPUDescriptorHandleInfo handleInfo = GetCPUHandleInfo();
    
    // デバイスを取得してUAVを作成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    heap_->GetDevice(IID_PPV_ARGS(&device));
    device->CreateUnorderedAccessView(resource, counterResource, desc, handleInfo.handle);

    return handleInfo;
}

void SRVHeap::SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList) {
    if (!commandList) {
        assert(false && "CommandList is null.");
        return;
    }

    ID3D12DescriptorHeap* heaps[] = { heap_.Get() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);
}

} // namespace KashipanEngine