#pragma once
#include "Graphics/IGPUResource.h"

namespace KashipanEngine {

/// @brief インデックスバッファ用のGPUリソース
class IndexBufferResource : public IGPUResource {
public:
    IndexBufferResource(const std::string &name, UINT indexCount, DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT);
    ~IndexBufferResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // インデックスバッファ固有の機能
    void Map();
    void Unmap();
    void UpdateIndices16(const uint16_t *indices, UINT count);
    void UpdateIndices32(const uint32_t *indices, UINT count);
    void *GetMappedIndices() { return mappedIndices_; }
    void SetAsIndexBuffer();
    
    // インデックスバッファビュー取得
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    UINT GetIndexCount() const { return indexCount_; }
    DXGI_FORMAT GetIndexFormat() const { return indexFormat_; }

protected:
    void RecreateResource() override;

private:
    // インデックス数
    UINT indexCount_;
    
    // インデックスフォーマット
    DXGI_FORMAT indexFormat_;
    
    // マップされたインデックスデータへのポインタ
    void *mappedIndices_ = nullptr;
    
    // インデックスバッファビュー
    D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
    
    // インデックスバッファのサイズ
    size_t GetBufferSize() const;
    size_t GetIndexSize() const;
};

} // namespace KashipanEngine