#include "DSVHeap.h"

namespace KashipanEngine {

DSVHeap::DSVHeap(ID3D12Device* device, UINT numDescriptors)
    : DescriptorHeapBase(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {
}

CPUDescriptorHandleInfo DSVHeap::CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc) {
    if (!resource) {
        assert(false && "Resource is null.");
        return CPUDescriptorHandleInfo({}, 0);
    }

    // CPUハンドル情報を取得
    CPUDescriptorHandleInfo handleInfo = GetCPUHandleInfo();
    
    // デバイスを取得してDSVを作成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    heap_->GetDevice(IID_PPV_ARGS(&device));
    device->CreateDepthStencilView(resource, desc, handleInfo.handle);

    return handleInfo;
}

} // namespace KashipanEngine