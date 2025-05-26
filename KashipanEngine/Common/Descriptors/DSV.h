#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <source_location>

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;

/*
シングルトンでどこからでもアクセスできてしまうため、
必ずどこから呼び出されたかを特定するために引数にsource_locationを設定
*/

/// @brief 深度ステンシルビュー用クラス
class DSV {
public:
    DSV(const DSV &) = delete;
    DSV(const DSV &&) = delete;
    DSV &operator=(const DSV &) = delete;
    DSV &operator=(const DSV &&) = delete;

    /// @brief 初期化処理
    /// @param winApp WinAppインスタンスへのポインタ
    /// @param dxCommon DirectXCommonインスタンスへのポインタ
    static void Initialize(
        WinApp *winApp,
        DirectXCommon *dxCommon,
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 終了処理
    static void Finalize(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief DescriptorHeapの取得
    /// @return DescriptorHeapのポインタ
    [[nodiscard]] static ID3D12DescriptorHeap *GetDescriptorHeap(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief DepthStencilResourceの取得
    /// @return DepthStencilResourceのポインタ
    [[nodiscard]] static ID3D12Resource *GetDepthStencilResource(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief CPUディスクリプタハンドルの取得 
    /// @return CPUディスクリプタハンドル
    [[nodiscard]] static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief GPUディスクリプタハンドルの取得
    /// @return GPUディスクリプタハンドル
    [[nodiscard]] static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 指定の位置のCPUディスクリプタハンドルの取得
    /// @param index インデックス
    /// @return CPUディスクリプタハンドル
    [[nodiscard]] static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
        const uint32_t index,
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 指定の位置のGPUディスクリプタハンドルの取得
    /// param index インデックス
    /// @return GPUディスクリプタハンドル
    [[nodiscard]] static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
        const uint32_t index,
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 次に使用するCPUディスクリプタハンドルのインデックスを取得
    /// @return 次に使用するCPUディスクリプタハンドルのインデックス
    [[nodiscard]] static uint32_t GetNextIndexCPU(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 次に使用するGPUディスクリプタハンドルのインデックスを取得
    /// @return 次に使用するGPUディスクリプタハンドルのインデックス
    [[nodiscard]] static uint32_t GetNextIndexGPU(
        const std::source_location &location = std::source_location::current()
    );

private:
    DSV() = default;
    ~DSV() = default;

    /// @brief 初期化フラグ
    static bool isInitialized_;
    /// @brief DirectXCommonインスタンス
    static DirectXCommon *dxCommon_;

    /// @brief ディスクリプタの数
    static const int numDescriptors_ = 2;
    /// @brief ディスクリプタヒープ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    /// @brief 深度ステンシルビュー用のリソース
    static Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    /// @brief 次に使用するCPUディスクリプタハンドルのインデックス
    static uint32_t nextIndexCPU_;
    /// @brief 次に使用するGPUディスクリプタハンドルのインデックス
    static uint32_t nextIndexGPU_;
};

} // namespace KashipanEngine