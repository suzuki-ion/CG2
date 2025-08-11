#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include <cassert>
#include <string>

namespace KashipanEngine {

struct RootSignatureStruct {
    std::string rootSignatureName;
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

class RootSignature {
public:
    RootSignature() = delete;
    /// @brief コンストラクタ
    /// @param device D3D12デバイス
    RootSignature(ID3D12Device *device) : device_(device) {
        if (!device_) {
            assert(false);
        }
    }
    ~RootSignature() = default;

    [[nodiscard]] ID3D12RootSignature *operator[](const std::string &rootSignatureName) const {
        return GetRootSignatureBinary(rootSignatureName);
    }

    /// @brief ルートシグネチャの追加
    /// @param rootSignatureName ルートシグネチャ名
    /// @param rootSignatures ルートシグネチャ
    void AddRootSignature(const std::string &rootSignatureName, const D3D12_ROOT_SIGNATURE_DESC &rootSignature) {
        RootSignatureStruct rootSignatureStruct;
        rootSignatureStruct.rootSignatureName = rootSignatureName;
        rootSignatureStruct.rootSignatureDesc = rootSignature;
        rootSignatures_[rootSignatureName] = rootSignatureStruct;
    }

    /// @brief ルートシグネチャの設定
    /// @param rootSignatureName ルートシグネチャ名
    /// @param rootParameters ルートパラメータの配列
    /// @param staticSamplers 静的サンプラーの配列
    /// @param flags ルートシグネチャフラグ
    void SetRootSignature(
        const std::string &rootSignatureName,
        const std::vector<D3D12_ROOT_PARAMETER> &rootParameters,
        const std::vector<D3D12_STATIC_SAMPLER_DESC> &staticSamplers = {},
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
    );

    /// @brief ルートシグネチャ(バイナリ)の取得
    /// @param rootSignatureName ルートシグネチャ名
    /// @return ルートシグネチャのバイナリ
    [[nodiscard]] ID3D12RootSignature *GetRootSignatureBinary(const std::string &rootSignatureName) const {
        auto it = rootSignatures_.find(rootSignatureName);
        if (it != rootSignatures_.end()) {
            return it->second.rootSignature.Get();
        }
        return nullptr;
    }

    /// @brief ルートシグネチャのクリア
    void Clear() {
        rootSignatures_.clear();
    }

private:
    ID3D12Device *device_ = nullptr;
    std::unordered_map<std::string, RootSignatureStruct> rootSignatures_;
};

} // namespace KashipanEngine