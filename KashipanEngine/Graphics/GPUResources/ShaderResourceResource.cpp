#include "ShaderResourceResource.h"
#include "Core/DirectX/DirectXCommon.h"
#include "Common/Descriptors/SRV.h"
#include <cassert>

namespace KashipanEngine {

ShaderResourceResource::ShaderResourceResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format)
    : IGPUResource(name, ResourceViewType::SRV), useExistingResource_(false) {
    width_ = width;
    height_ = height;
    format_ = format;
}

ShaderResourceResource::ShaderResourceResource(const std::string &name, Microsoft::WRL::ComPtr<ID3D12Resource> existingResource)
    : IGPUResource(name, ResourceViewType::SRV), useExistingResource_(true) {
    resource_ = existingResource;
    
    if (resource_) {
        auto desc = resource_->GetDesc();
        width_ = static_cast<UINT>(desc.Width);
        height_ = desc.Height;
        format_ = desc.Format;
    }
}

void ShaderResourceResource::Create() {
    assert(isCommonInitialized_);
    
    if (!useExistingResource_) {
        // 新しいリソースを作成
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
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        resource_ = CreateCommittedResource(
            heapProps,
            D3D12_HEAP_FLAG_NONE,
            resourceDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr
        );
        
        currentState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
    
    // SRV作成
    CreateShaderResourceView();
}

void ShaderResourceResource::Release() {
    if (!useExistingResource_) {
        resource_.Reset();
    }
    srvHandleCPU_ = {};
    srvHandleGPU_ = {};
}

void ShaderResourceResource::RecreateResource() {
    if (!useExistingResource_) {
        Release();
        Create();
    } else {
        // 既存リソースの場合はビューのみ再作成
        CreateShaderResourceView();
    }
}

void ShaderResourceResource::CreateShaderResourceView() {
    assert(resource_);
    
    // ディスクリプタハンドル取得
    srvHandleCPU_ = SRV::GetCPUDescriptorHandle();
    srvHandleGPU_ = SRV::GetGPUDescriptorHandle();
    
    // SRV作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format_;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    
    dxCommon_->GetDevice()->CreateShaderResourceView(
        resource_.Get(),
        &srvDesc,
        srvHandleCPU_
    );
}

void ShaderResourceResource::SetAsShaderResource(UINT rootParameterIndex) {
    assert(resource_);
    
    // シェーダーリソース状態に遷移
    TransitionTo(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    
    // ディスクリプタテーブルを設定
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(
        rootParameterIndex,
        srvHandleGPU_
    );
}

} // namespace KashipanEngine