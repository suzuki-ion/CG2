#include "IGPUResource.h"
#include "Core/DirectX/DirectXDevice.h"
#include "Core/DirectX/DirectXCommon.h"
#include <cassert>

namespace KashipanEngine {

// 静的メンバの定義
DirectXDevice *IGPUResource::dxDevice_ = nullptr;
bool IGPUResource::isCommonInitialized_ = false;

void IGPUResource::InitializeCommon(DirectXDevice *dxDevice) {
    assert(dxDevice != nullptr);
    dxDevice_ = dxDevice;
    isCommonInitialized_ = true;
}

void IGPUResource::TransitionTo(D3D12_RESOURCE_STATES newState) {
    if (currentState_ != newState && resource_) {
        SetResourceBarrier(currentState_, newState);
        currentState_ = newState;
    }
}

void IGPUResource::SetResourceBarrier(D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState) {
    assert(isCommonInitialized_);
    assert(resource_);
    
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = beforeState;
    barrier.Transition.StateAfter = afterState;
    
    dxDevice_->GetCommandList()->ResourceBarrier(1, &barrier);
}

Microsoft::WRL::ComPtr<ID3D12Resource> IGPUResource::CreateCommittedResource(
    const D3D12_HEAP_PROPERTIES &heapProps,
    D3D12_HEAP_FLAGS heapFlags,
    const D3D12_RESOURCE_DESC &resourceDesc,
    D3D12_RESOURCE_STATES initialState,
    const D3D12_CLEAR_VALUE *clearValue) {
    
    assert(isCommonInitialized_);
    
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    HRESULT hr = dxDevice_->GetDevice()->CreateCommittedResource(
        &heapProps,
        heapFlags,
        &resourceDesc,
        initialState,
        clearValue,
        IID_PPV_ARGS(&resource)
    );
    
    assert(SUCCEEDED(hr));
    return resource;
}

UINT IGPUResource::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) const {
    assert(isCommonInitialized_);
    return dxDevice_->GetDevice()->GetDescriptorHandleIncrementSize(heapType);
}

} // namespace KashipanEngine