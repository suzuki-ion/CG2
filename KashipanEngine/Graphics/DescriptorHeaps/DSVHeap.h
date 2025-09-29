#pragma once
#include "../DescriptorHeapBase.h"

namespace KashipanEngine {

/// @brief DSV (Depth Stencil View) ディスクリプタヒープクラス
class DSVHeap : public DescriptorHeapBase {
public:
    /// @brief コンストラクタ
    /// @param device D3D12デバイス
    /// @param numDescriptors ディスクリプタ数
    DSVHeap(ID3D12Device* device, UINT numDescriptors = 256);

    /// @brief デストラクタ
    virtual ~DSVHeap() = default;

    /// @brief デプスステンシルビューを作成
    /// @param resource デプスステンシルリソース
    /// @param desc デプスステンシルビューの設定（nullptrの場合はデフォルト設定）
    /// @return 作成されたDSVのCPUハンドル情報
    CPUDescriptorHandleInfo CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc = nullptr);
};

} // namespace KashipanEngine