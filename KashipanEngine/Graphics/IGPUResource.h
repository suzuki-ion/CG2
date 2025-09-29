#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace KashipanEngine {

// 前方宣言
class DirectXDevice;

enum class ResourceViewType {
    None,   // 未使用
    RTV,    // レンダーターゲットビュー
    DSV,    // 深度ステンシルビュー
    SRV,    // シェーダーリソースビュー
    UAV,    // アンオーダードアクセスビュー
    CBV,    // 定数バッファビュー
    VBV,    // 頂点バッファビュー
    IBV,    // インデックスバッファビュー
    SOV,    // ストリーム出力ビュー
};

/// @brief GPUリソース用の抽象クラス
class IGPUResource {
public:
    static void Initialize(DirectXDevice *dxDevice) {
        dxDevice_ = dxDevice;
    }

    IGPUResource(const std::string &name, ResourceViewType viewType)
        : name_(name), viewType_(viewType), format_(DXGI_FORMAT_UNKNOWN) {
        resourceCount_++;
    }
    ~IGPUResource() {
        resource_ = nullptr;
        resourceCount_--;
    }
    
    const ID3D12Resource *GetResource() const { return resource_.Get(); }
    ID3D12Resource *GetResource() { return resource_.Get(); }
    ResourceViewType GetViewType() const { return viewType_; }
    const std::string &GetName() const { return name_; }
    DXGI_FORMAT GetFormat() const { return format_; }
    D3D12_RESOURCE_STATES GetCurrentState() const { return currentState_; }
    
    // リソースの生成
    virtual void Create() = 0;
    // リソースの解放
    virtual void Release() = 0;
    
    // リソースの状態変更
    virtual void TransitionTo(D3D12_RESOURCE_STATES newState);
    
protected:
    static DirectXDevice *dxDevice_;
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    const std::string name_;
    const ResourceViewType viewType_;
    DXGI_FORMAT format_;

    const D3D12_RESOURCE_STATES kFirstState_ = D3D12_RESOURCE_STATE_COMMON;
    const D3D12_RESOURCE_STATES kLastState_ = D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_COMMON;
    
private:
    // デバッグ用に作成されたリソースの数をカウントしておく
    static inline int32_t resourceCount_ = 0;
};

} // namespace KashipanEngine