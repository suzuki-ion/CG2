#pragma once
#include "Graphics/IGPUResource.h"
#include "Common/Descriptors/SRV.h"

namespace KashipanEngine {

/// @brief シェーダーリソース用のGPUリソース
class ShaderResourceResource : public IGPUResource {
public:
    ShaderResourceResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
    ShaderResourceResource(const std::string &name, Microsoft::WRL::ComPtr<ID3D12Resource> existingResource);
    ~ShaderResourceResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // シェーダーリソース固有の機能
    void SetAsShaderResource(UINT rootParameterIndex);
    
    // ディスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandleCPU() const { return srvHandleCPU_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU() const { return srvHandleGPU_; }

protected:
    void RecreateResource() override;

private:
    // ビュー作成
    void CreateShaderResourceView();
    
    // ディスクリプタハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_ = {};
    
    // 既存リソースを使用するかどうか
    bool useExistingResource_ = false;
};

} // namespace KashipanEngine