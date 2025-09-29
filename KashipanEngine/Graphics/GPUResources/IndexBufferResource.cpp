#include "IndexBufferResource.h"
#include "Core/DirectX/DirectXCommon.h"
#include <cassert>

namespace KashipanEngine {

IndexBufferResource::IndexBufferResource(const std::string &name, UINT indexCount, DXGI_FORMAT indexFormat)
    : IGPUResource(name, ResourceViewType::IBV), indexCount_(indexCount), indexFormat_(indexFormat) {
    width_ = indexCount;
    height_ = 1;
    format_ = indexFormat;
}

void IndexBufferResource::Create() {
    assert(isCommonInitialized_);
    
    // インデックスバッファのリソース作成
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = GetBufferSize();
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    resource_ = CreateCommittedResource(
        heapProps,
        D3D12_HEAP_FLAG_NONE,
        resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr
    );
    
    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    
    // インデックスバッファビューを設定
    indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast<UINT>(GetBufferSize());
    indexBufferView_.Format = indexFormat_;
    
    // マップ
    Map();
}

void IndexBufferResource::Release() {
    if (mappedIndices_) {
        Unmap();
    }
    resource_.Reset();
    indexBufferView_ = {};
}

void IndexBufferResource::RecreateResource() {
    Release();
    Create();
}

void IndexBufferResource::Map() {
    assert(resource_);
    
    HRESULT hr = resource_->Map(0, nullptr, &mappedIndices_);
    assert(SUCCEEDED(hr));
}

void IndexBufferResource::Unmap() {
    if (resource_ && mappedIndices_) {
        resource_->Unmap(0, nullptr);
        mappedIndices_ = nullptr;
    }
}

void IndexBufferResource::UpdateIndices16(const uint16_t *indices, UINT count) {
    assert(mappedIndices_);
    assert(count <= indexCount_);
    assert(indexFormat_ == DXGI_FORMAT_R16_UINT);
    
    uint16_t *mappedIndices16 = static_cast<uint16_t*>(mappedIndices_);
    for (UINT i = 0; i < count; ++i) {
        mappedIndices16[i] = indices[i];
    }
}

void IndexBufferResource::UpdateIndices32(const uint32_t *indices, UINT count) {
    assert(mappedIndices_);
    assert(count <= indexCount_);
    assert(indexFormat_ == DXGI_FORMAT_R32_UINT);
    
    uint32_t *mappedIndices32 = static_cast<uint32_t*>(mappedIndices_);
    for (UINT i = 0; i < count; ++i) {
        mappedIndices32[i] = indices[i];
    }
}

void IndexBufferResource::SetAsIndexBuffer() {
    assert(resource_);
    
    dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
}

D3D12_INDEX_BUFFER_VIEW IndexBufferResource::GetIndexBufferView() const {
    return indexBufferView_;
}

size_t IndexBufferResource::GetBufferSize() const {
    return GetIndexSize() * indexCount_;
}

size_t IndexBufferResource::GetIndexSize() const {
    switch (indexFormat_) {
        case DXGI_FORMAT_R16_UINT:
            return sizeof(uint16_t);
        case DXGI_FORMAT_R32_UINT:
            return sizeof(uint32_t);
        default:
            assert(false && "Unsupported index format");
            return 0;
    }
}

} // namespace KashipanEngine