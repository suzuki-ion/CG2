#pragma once
#include "Graphics/IGPUResource.h"
#include "Common/Descriptors/SRV.h" // UAVもSRVヒープに含まれることが多い

namespace KashipanEngine {

/// @brief アンオーダードアクセス用のGPUリソース
class UnorderedAccessResource : public IGPUResource {
public:
    UnorderedAccessResource(const std::string &name, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
    ~UnorderedAccessResource() override = default;

    // IGPUResource インターフェースの実装
    void Create() override;
    void Release() override;

    // UAV固有の機能
    void SetAsUnorderedAccess(UINT rootParameterIndex);
    void ClearUAV(const UINT clearValues[4]);
    void ClearUAV(const FLOAT clearValues[4]);
    
    // ディスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandleCPU() const { return uavHandleCPU_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetUAVHandleGPU() const { return uavHandleGPU_; }

protected:
    void RecreateResource() override;

private:
    // ビュー作成
    void CreateUnorderedAccessView();
    
    // ディスクリプタハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU_ = {};
};

} // namespace KashipanEngine