#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include "VertexData.h"

namespace KashipanEngine {

// メッシュ
struct Mesh {
    ~Mesh() {
        // 頂点バッファのアンマップ
        if (vertexBuffer.Get()) {
            vertexBuffer->Unmap(0, nullptr);
            vertexBufferMap = nullptr;
        }
        // インデックスバッファのアンマップ
        if (indexBuffer.Get()) {
            indexBuffer->Unmap(0, nullptr);
            indexBufferMap = nullptr;
        }
    }

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // インデックスバッファビュー
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    // 頂点バッファマップ
    VertexData *vertexBufferMap = nullptr;
    // インデックスバッファマップ
    uint32_t *indexBufferMap = nullptr;
};

} // namespace KashipanEngine