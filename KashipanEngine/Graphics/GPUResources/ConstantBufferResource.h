#pragma once
#include "Graphics/IGPUResource.h"

namespace KashipanEngine {

/// @brief 定数バッファ用のGPUリソース
template<typename T>
class ConstantBufferResource : public IGPUResource {
public:
    ConstantBufferResource(const std::string &name);
    ~ConstantBufferResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // 定数バッファ固有の機能
    void Map();
    void Unmap();
    void UpdateData(const T &data);
    T *GetMappedData() { return mappedData_; }
    void SetAsConstantBuffer(UINT rootParameterIndex);
    
    // GPUアドレス取得
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

protected:
    void RecreateResource() override;

private:
    // マップされたデータへのポインタ
    T *mappedData_ = nullptr;
    
    // 定数バッファのサイズ（256バイトアライメント）
    static constexpr size_t kConstantBufferSize = (sizeof(T) + 255) & ~255;
};

// 特殊化されていないテンプレートの実装
template<typename T>
ConstantBufferResource<T>::ConstantBufferResource(const std::string &name)
    : IGPUResource(name, ResourceViewType::CBV) {
    width_ = 1;
    height_ = 1;
    format_ = DXGI_FORMAT_UNKNOWN;
}

template<typename T>
void ConstantBufferResource<T>::Create() {
    assert(isCommonInitialized_);
    
    // 定数バッファのリソース作成
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = kConstantBufferSize;
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
    
    // マップ
    Map();
}

template<typename T>
void ConstantBufferResource<T>::Release() {
    if (mappedData_) {
        Unmap();
    }
    resource_.Reset();
}

template<typename T>
void ConstantBufferResource<T>::RecreateResource() {
    Release();
    Create();
}

template<typename T>
void ConstantBufferResource<T>::Map() {
    assert(resource_);
    
    HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
    assert(SUCCEEDED(hr));
}

template<typename T>
void ConstantBufferResource<T>::Unmap() {
    if (resource_ && mappedData_) {
        resource_->Unmap(0, nullptr);
        mappedData_ = nullptr;
    }
}

template<typename T>
void ConstantBufferResource<T>::UpdateData(const T &data) {
    assert(mappedData_);
    *mappedData_ = data;
}

template<typename T>
void ConstantBufferResource<T>::SetAsConstantBuffer(UINT rootParameterIndex) {
    assert(resource_);
    
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(
        rootParameterIndex,
        GetGPUVirtualAddress()
    );
}

template<typename T>
D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferResource<T>::GetGPUVirtualAddress() const {
    assert(resource_);
    return resource_->GetGPUVirtualAddress();
}

} // namespace KashipanEngine