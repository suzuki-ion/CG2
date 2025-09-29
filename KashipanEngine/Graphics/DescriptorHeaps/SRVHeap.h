#pragma once
#include "../DescriptorHeapBase.h"

namespace KashipanEngine {

/// @brief SRV (Shader Resource View) ディスクリプタヒープクラス
/// CBV、SRV、UAVを含むディスクリプタヒープ
class SRVHeap : public DescriptorHeapBase {
public:
    /// @brief コンストラクタ
    /// @param device D3D12デバイス
    /// @param numDescriptors ディスクリプタ数
    SRVHeap(ID3D12Device* device, UINT numDescriptors = 1024);

    /// @brief デストラクタ
    virtual ~SRVHeap() = default;

    /// @brief シェーダーリソースビューを作成
    /// @param resource シェーダーリソース
    /// @param desc シェーダーリソースビューの設定（nullptrの場合はデフォルト設定）
    /// @return 作成されたSRVのCPUハンドル情報
    CPUDescriptorHandleInfo CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc = nullptr);

    /// @brief コンスタントバッファビューを作成
    /// @param desc コンスタントバッファビューの設定
    /// @return 作成されたCBVのCPUハンドル情報
    CPUDescriptorHandleInfo CreateCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    /// @brief アンオーダードアクセスビューを作成
    /// @param resource UAVリソース
    /// @param counterResource カウンターリソース（nullptrの場合はカウンターなし）
    /// @param desc アンオーダードアクセスビューの設定（nullptrの場合はデフォルト設定）
    /// @return 作成されたUAVのCPUハンドル情報
    CPUDescriptorHandleInfo CreateUAV(ID3D12Resource* resource, ID3D12Resource* counterResource = nullptr, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc = nullptr);

    /// @brief ディスクリプタヒープをコマンドリストに設定
    /// @param commandList コマンドリスト
    void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList);
};

} // namespace KashipanEngine