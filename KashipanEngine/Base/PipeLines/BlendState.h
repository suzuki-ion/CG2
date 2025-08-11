#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace KashipanEngine {

class BlendState {
public:
    [[nodiscard]] const D3D12_BLEND_DESC &operator[](const std::string &blendName) const {
        return GetBlendState(blendName);
    }

    /// @brief ブレンドステート追加
    /// @param blendName ブレンド名
    /// @param blendDesc ブレンドステート
    void AddBlendState(const std::string &blendName, D3D12_BLEND_DESC blendDesc) {
        blendDescs_[blendName] = blendDesc;
    }

    /// @brief ブレンドステート
    /// @param blendName ブレンド名
    /// @return ブレンドステートの参照
    [[nodiscard]] const D3D12_BLEND_DESC &GetBlendState(const std::string &blendName) const {
        return blendDescs_.at(blendName);
    }

    /// @brief ブレンドステートのクリア
    void Clear() {
        blendDescs_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_BLEND_DESC> blendDescs_;
};

} // namespace KashipanEngine