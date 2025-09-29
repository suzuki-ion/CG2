#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

namespace KashipanEngine {

// 前方宣言
class DirectXDevice;

/// @brief リソース作成用ファクトリクラス
/// @details 各種D3D12リソースの作成を担当
class ResourceFactory final {
public:
    explicit ResourceFactory(DirectXDevice* device);
    ~ResourceFactory() = default;

    // コピー・ムーブ禁止
    ResourceFactory(const ResourceFactory&) = delete;
    ResourceFactory& operator=(const ResourceFactory&) = delete;
    ResourceFactory(ResourceFactory&&) = delete;
    ResourceFactory& operator=(ResourceFactory&&) = delete;

    /// @brief 深度ステンシルテクスチャリソースを作成
    /// @param width テクスチャの幅
    /// @param height テクスチャの高さ
    /// @param format フォーマット
    /// @param initialState 初期状態
    /// @return 作成されたリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
        int32_t width,
        int32_t height,
        DXGI_FORMAT format,
        D3D12_RESOURCE_STATES initialState
    );

    /// @brief テクスチャリソースを作成
    /// @param width テクスチャの幅
    /// @param height テクスチャの高さ
    /// @param format フォーマット
    /// @param flags リソースフラグ
    /// @param initialState 初期状態
    /// @param clearValue クリア値（オプション）
    /// @return 作成されたリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
        int32_t width,
        int32_t height,
        DXGI_FORMAT format,
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_GENERIC_READ,
        const D3D12_CLEAR_VALUE* clearValue = nullptr
    );

    /// @brief バッファリソースを作成
    /// @param size バッファサイズ
    /// @param heapType ヒープタイプ
    /// @param initialState 初期状態
    /// @return 作成されたリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        size_t size,
        D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_GENERIC_READ
    );

private:
    DirectXDevice* device_;
};

} // namespace KashipanEngine