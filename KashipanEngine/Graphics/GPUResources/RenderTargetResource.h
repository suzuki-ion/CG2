#pragma once
#include "Graphics/IGPUResource.h"
#include "Common/Descriptors/RTV.h"

namespace KashipanEngine {

/// @brief レンダーターゲット用のGPUリソース
class RenderTargetResource : public IGPUResource {
public:
    RenderTargetResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
    ~RenderTargetResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // レンダーターゲット固有の機能
    void Clear(const float clearColor[4]);
    void SetAsRenderTarget();
    
    // ディスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandleCPU() const { return rtvHandleCPU_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRTVHandleGPU() const { return rtvHandleGPU_; }

protected:
    void RecreateResource() override;

private:
    // リソース作成
    void CreateRenderTargetView();
    
    // ディスクリプタハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE rtvHandleGPU_ = {};
    
    // クリア値
    D3D12_CLEAR_VALUE clearValue_ = {};
};

} // namespace KashipanEngine
