#include "UnorderedAccessResource.h"
#include "Core/DirectX/DirectXCommon.h"
#include "Common/Descriptors/SRV.h"
#include <cassert>

namespace KashipanEngine {

UnorderedAccessResource::UnorderedAccessResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format)
    : IGPUResource(name, ResourceViewType::UAV) {
    width_ = width;
    height_ = height;
    format_ = format;
}

void UnorderedAccessResource::Create() {
    assert(isCommonInitialized_);
    
    // リソース作成
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = width_;
    resourceDesc.Height = height_;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = format_;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    resource_ = CreateCommittedResource(
        heapProps,
        D3D12_HEAP_FLAG_NONE,
        resourceDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr
    );
    
    currentState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    
    // UAV作成
    CreateUnorderedAccessView();
}

void UnorderedAccessResource::Release() {
    resource_.Reset();
    uavHandleCPU_ = {};
    uavHandleGPU_ = {};
}

void UnorderedAccessResource::RecreateResource() {
    Release();
    Create();
}

void UnorderedAccessResource::CreateUnorderedAccessView() {
    assert(resource_);
    
    // ディスクリプタハンドル取得（UAVもSRVヒープに含まれる）
    uavHandleCPU_ = SRV::GetCPUDescriptorHandle();
    uavHandleGPU_ = SRV::GetGPUDescriptorHandle();
    
    // UAV作成
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = format_;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    uavDesc.Texture2D.PlaneSlice = 0;
    
    dxCommon_->GetDevice()->CreateUnorderedAccessView(
        resource_.Get(),
        nullptr, // カウンターリソースなし
        &uavDesc,
        uavHandleCPU_
    );
}

void UnorderedAccessResource::SetAsUnorderedAccess(UINT rootParameterIndex) {
    assert(resource_);
    
    // UAV状態に遷移
    TransitionTo(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    
    // ディスクリプタテーブルを設定
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(
        rootParameterIndex,
        uavHandleGPU_
    );
}

void UnorderedAccessResource::ClearUAV(const UINT clearValues[4]) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->ClearUnorderedAccessViewUint(
        uavHandleGPU_,
        uavHandleCPU_,
        resource_.Get(),
        clearValues,
        0,
        nullptr
    );
}

void UnorderedAccessResource::ClearUAV(const FLOAT clearValues[4]) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->ClearUnorderedAccessViewFloat(
        uavHandleGPU_,
        uavHandleCPU_,
        resource_.Get(),
        clearValues,
        0,
        nullptr
    );
}

} // namespace KashipanEngine