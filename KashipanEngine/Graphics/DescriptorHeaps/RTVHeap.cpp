#include "RTVHeap.h"

namespace KashipanEngine {

RTVHeap::RTVHeap(ID3D12Device* device, UINT numDescriptors)
    : DescriptorHeapBase(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {
}

CPUDescriptorHandleInfo RTVHeap::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc) {
    if (!resource) {
        assert(false && "Resource is null.");
        return CPUDescriptorHandleInfo({}, 0);
    }

    // CPUハンドル情報を取得
    CPUDescriptorHandleInfo handleInfo = GetCPUHandleInfo();
    
    // デバイスを取得してRTVを作成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    heap_->GetDevice(IID_PPV_ARGS(&device));
    device->CreateRenderTargetView(resource, desc, handleInfo.handle);

    return handleInfo;
}

} // namespace KashipanEngine