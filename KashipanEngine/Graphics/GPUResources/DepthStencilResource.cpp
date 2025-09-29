#include "DepthStencilResource.h"
#include "Core/DirectX/DirectXCommon.h"
#include <cassert>

namespace KashipanEngine {

DepthStencilResource::DepthStencilResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format)
    : IGPUResource(name, ResourceViewType::DSV) {
    width_ = width;
    height_ = height;
    format_ = format;
    
    // クリア値を設定
    clearValue_.Format = format_;
    clearValue_.DepthStencil.Depth = 1.0f;
    clearValue_.DepthStencil.Stencil = 0;
}

void DepthStencilResource::Create() {
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
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    resource_ = CreateCommittedResource(
        heapProps,
        D3D12_HEAP_FLAG_NONE,
        resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue_
    );
    
    currentState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    
    // DSV作成
    CreateDepthStencilView();
}

void DepthStencilResource::Release() {
    resource_.Reset();
    dsvHandleCPU_ = {};
    dsvHandleGPU_ = {};
}

void DepthStencilResource::RecreateResource() {
    Release();
    Create();
}

void DepthStencilResource::CreateDepthStencilView() {
    assert(resource_);
    
    // ディスクリプタハンドル取得
    dsvHandleCPU_ = DSV::GetCPUDescriptorHandle();
    dsvHandleGPU_ = DSV::GetGPUDescriptorHandle();
    
    // DSV作成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = format_;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    
    dxCommon_->GetDevice()->CreateDepthStencilView(
        resource_.Get(),
        &dsvDesc,
        dsvHandleCPU_
    );
}

void DepthStencilResource::Clear(float depth, UINT8 stencil) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->ClearDepthStencilView(
        dsvHandleCPU_,
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        depth,
        stencil,
        0,
        nullptr
    );
}

void DepthStencilResource::SetAsDepthStencil() {
    assert(resource_);
    
    // 深度書き込み状態に遷移
    TransitionTo(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

} // namespace KashipanEngine