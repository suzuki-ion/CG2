#include "RenderTargetResource.h"
#include "Core/DirectX/DirectXCommon.h"
#include <cassert>

namespace KashipanEngine {

RenderTargetResource::RenderTargetResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format)
    : IGPUResource(name, ResourceViewType::RTV) {
    width_ = width;
    height_ = height;
    format_ = format;
    
    // クリア値を設定
    clearValue_.Format = format_;
    clearValue_.Color[0] = 0.0f;
    clearValue_.Color[1] = 0.0f;
    clearValue_.Color[2] = 0.0f;
    clearValue_.Color[3] = 1.0f;
}

void RenderTargetResource::Create() {
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
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    resource_ = CreateCommittedResource(
        heapProps,
        D3D12_HEAP_FLAG_NONE,
        resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue_
    );
    
    currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    // RTV作成
    CreateRenderTargetView();
}

void RenderTargetResource::Release() {
    resource_.Reset();
    rtvHandleCPU_ = {};
    rtvHandleGPU_ = {};
}

void RenderTargetResource::RecreateResource() {
    Release();
    Create();
}

void RenderTargetResource::CreateRenderTargetView() {
    assert(resource_);
    
    // ディスクリプタハンドル取得
    rtvHandleCPU_ = RTV::GetCPUDescriptorHandle();
    rtvHandleGPU_ = RTV::GetGPUDescriptorHandle();
    
    // RTV作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = format_;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    
    dxCommon_->GetDevice()->CreateRenderTargetView(
        resource_.Get(),
        &rtvDesc,
        rtvHandleCPU_
    );
}

void RenderTargetResource::Clear(const float clearColor[4]) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->ClearRenderTargetView(
        rtvHandleCPU_,
        clearColor,
        0,
        nullptr
    );
}

void RenderTargetResource::SetAsRenderTarget() {
    assert(resource_);
    
    // レンダーターゲット状態に遷移
    TransitionTo(D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    // レンダーターゲットを設定
    dxCommon_->GetCommandList()->OMSetRenderTargets(
        1,
        &rtvHandleCPU_,
        FALSE,
        nullptr
    );
}

} // namespace KashipanEngine