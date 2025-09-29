#pragma once
#include "../DescriptorHeapBase.h"

namespace KashipanEngine {

/// @brief RTV (Render Target View) ディスクリプタヒープクラス
class RTVHeap : public DescriptorHeapBase {
public:
    /// @brief コンストラクタ
    /// @param device D3D12デバイス
    /// @param numDescriptors ディスクリプタ数
    RTVHeap(ID3D12Device* device, UINT numDescriptors = 256);

    /// @brief デストラクタ
    virtual ~RTVHeap() = default;

    /// @brief レンダーターゲットビューを作成
    /// @param resource レンダーターゲットリソース
    /// @param desc レンダーターゲットビューの設定（nullptrの場合はデフォルト設定）
    /// @return 作成されたRTVのCPUハンドル情報
    CPUDescriptorHandleInfo CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr);
};

} // namespace KashipanEngine