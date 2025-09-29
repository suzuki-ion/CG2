#pragma once
#include "Graphics/IGPUResource.h"

namespace KashipanEngine {

/// @brief 頂点バッファ用のGPUリソース
template<typename VertexType>
class VertexBufferResource : public IGPUResource {
public:
    VertexBufferResource(const std::string &name, UINT vertexCount);
    ~VertexBufferResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // 頂点バッファ固有の機能
    void Map();
    void Unmap();
    void UpdateVertices(const VertexType *vertices, UINT count);
    VertexType *GetMappedVertices() { return mappedVertices_; }
    void SetAsVertexBuffer(UINT slot = 0);
    
    // 頂点バッファビュー取得
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
    UINT GetVertexCount() const { return vertexCount_; }

protected:
    void RecreateResource() override;

private:
    // 頂点数
    UINT vertexCount_;
    
    // マップされた頂点データへのポインタ
    VertexType *mappedVertices_ = nullptr;
    
    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
    
    // 頂点バッファのサイズ
    size_t GetBufferSize() const { return sizeof(VertexType) * vertexCount_; }
};

// テンプレートの実装
template<typename VertexType>
VertexBufferResource<VertexType>::VertexBufferResource(const std::string &name, UINT vertexCount)
    : IGPUResource(name, ResourceViewType::VBV), vertexCount_(vertexCount) {
    width_ = vertexCount;
    height_ = 1;
    format_ = DXGI_FORMAT_UNKNOWN;
}

template<typename VertexType>
void VertexBufferResource<VertexType>::Create() {
    assert(isCommonInitialized_);
    
    // 頂点バッファのリソース作成
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
    
    // 頂点バッファビューを設定
    vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(GetBufferSize());
    vertexBufferView_.StrideInBytes = sizeof(VertexType);
    
    // マップ
    Map();
}

template<typename VertexType>
void VertexBufferResource<VertexType>::Release() {
    if (mappedVertices_) {
        Unmap();
    }
    resource_.Reset();
    vertexBufferView_ = {};
}

template<typename VertexType>
void VertexBufferResource<VertexType>::RecreateResource() {
    Release();
    Create();
}

template<typename VertexType>
void VertexBufferResource<VertexType>::Map() {
    assert(resource_);
    
    HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertices_));
    assert(SUCCEEDED(hr));
}

template<typename VertexType>
void VertexBufferResource<VertexType>::Unmap() {
    if (resource_ && mappedVertices_) {
        resource_->Unmap(0, nullptr);
        mappedVertices_ = nullptr;
    }
}

template<typename VertexType>
void VertexBufferResource<VertexType>::UpdateVertices(const VertexType *vertices, UINT count) {
    assert(mappedVertices_);
    assert(count <= vertexCount_);
    
    for (UINT i = 0; i < count; ++i) {
        mappedVertices_[i] = vertices[i];
    }
}

template<typename VertexType>
void VertexBufferResource<VertexType>::SetAsVertexBuffer(UINT slot) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->IASetVertexBuffers(
        slot,
        1,
        &vertexBufferView_
    );
}

template<typename VertexType>
D3D12_VERTEX_BUFFER_VIEW VertexBufferResource<VertexType>::GetVertexBufferView() const {
    return vertexBufferView_;
}

} // namespace KashipanEngine