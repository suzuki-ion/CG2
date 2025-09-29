#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace KashipanEngine {

/// @brief DirectXデバイス管理クラス
/// @details DXGI、デバイス、コマンドキュー/アロケータ/リストの基盤機能を管理
class DirectXDevice final {
public:
    DirectXDevice(bool enableDebugLayer = true);
    ~DirectXDevice();

    // コピー・ムーブ禁止
    DirectXDevice(const DirectXDevice &) = delete;
    DirectXDevice &operator=(const DirectXDevice &) = delete;
    DirectXDevice(DirectXDevice &&) = delete;
    DirectXDevice &operator=(DirectXDevice &&) = delete;

    /// @brief D3D12デバイス取得
    /// @return D3D12デバイス
    ID3D12Device *GetDevice() const { return device_.Get(); }

    /// @brief コマンドキュー取得
    /// @return コマンドキュー
    ID3D12CommandQueue *GetCommandQueue() const { return commandQueue_.Get(); }

    /// @brief コマンドアロケータ取得
    /// @return コマンドアロケータ
    ID3D12CommandAllocator *GetCommandAllocator() const { return commandAllocator_.Get(); }

    /// @brief コマンドリスト取得
    /// @return コマンドリスト
    ID3D12GraphicsCommandList *GetCommandList() const { return commandList_.Get(); }

    /// @brief DXGIファクトリー取得
    /// @return DXGIファクトリー
    IDXGIFactory7 *GetDXGIFactory() const { return dxgiFactory_.Get(); }

    /// @brief フェンス取得
    /// @return フェンス
    ID3D12Fence *GetFence() const { return fence_.Get(); }

    /// @brief コマンドリストをリセット
    void ResetCommandList();

    /// @brief コマンドを実行（フェンス待機込み）
    void ExecuteCommand();

    /// @brief フェンス値を取得
    /// @return 現在のフェンス値
    UINT64 GetFenceValue() const { return fenceValue_; }

    /// @brief ディスクリプタヒープを作成
    /// @param descriptorHeap 作成されるディスクリプタヒープ
    /// @param heapType ヒープの種類
    /// @param numDescriptors ディスクリプタの数
    /// @param shaderVisible シェーダーから見えるかどうか
    void CreateDescriptorHeap(
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible = false
    );

private:
    /// @brief DXGI初期化
    void InitializeDXGI();

    /// @brief DXGIアダプター初期化
    void InitializeDXGIAdapter();

    /// @brief D3D12デバイス初期化
    void InitializeD3D12Device();

    /// @brief コマンドキュー初期化
    void InitializeCommandQueue();

    /// @brief コマンドアロケータ初期化
    void InitializeCommandAllocator();

    /// @brief コマンドリスト初期化
    void InitializeCommandList();

    /// @brief フェンス初期化
    void InitializeFence();

    /// @brief デバッグレイヤー設定
    void SetupDebugLayer(bool enableDebugLayer);

private:
    // DXGI関連
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

    // D3D12デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    // コマンド関連
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    // フェンス関連
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    UINT64 fenceValue_;
    HANDLE fenceEvent_;
};

} // namespace KashipanEngine