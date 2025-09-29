#pragma once
#include "Graphics/IGPUResource.h"
#include "Common/Descriptors/DSV.h"

namespace KashipanEngine {

/// @brief 深度ステンシル用のGPUリソース
class DepthStencilResource : public IGPUResource {
public:
    DepthStencilResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
    ~DepthStencilResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // 深度ステンシル固有の機能
    void Clear(float depth = 1.0f, UINT8 stencil = 0);
    void SetAsDepthStencil();
    
    // ディスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandleCPU() const { return dsvHandleCPU_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDSVHandleGPU() const { return dsvHandleGPU_; }

protected:
    void RecreateResource() override;

private:
    // ビュー作成
    void CreateDepthStencilView();
    
    // ディスクリプタハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE dsvHandleGPU_ = {};
    
    // クリア値
    D3D12_CLEAR_VALUE clearValue_ = {};
};

} // namespace KashipanEngine