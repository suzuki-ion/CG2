#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

class Sampler {
public:
    [[nodiscard]] const std::vector<D3D12_STATIC_SAMPLER_DESC> &operator[](const std::string &samplerName) const {
        return samplers_.at(samplerName);
    }

    /// @brief サンプラー追加
    /// @param samplerName サンプラー名
    /// @param samplers サンプラー
    void AddSampler(const std::string &samplerName, const std::vector<D3D12_STATIC_SAMPLER_DESC> &samplers) {
        samplers_[samplerName] = samplers;
    }

    /// @brief サンプラー取得
    /// @param samplerName サンプラー名
    /// @return サンプラーの参照
    [[nodiscard]] const std::vector<D3D12_STATIC_SAMPLER_DESC> &GetSampler(const std::string &samplerName) const {
        return samplers_.at(samplerName);
    }

    /// @brief サンプラーのクリア
    void Clear() {
        samplers_.clear();
    }

private:
    std::unordered_map<std::string, std::vector<D3D12_STATIC_SAMPLER_DESC>> samplers_;
};

} // namespace KashipanEngine