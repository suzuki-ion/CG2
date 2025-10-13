#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace KashipanEngine {

class RasterizerState {
public:
    [[nodiscard]] const D3D12_RASTERIZER_DESC &operator[](const std::string &rasterizerName) const {
        return rasterizerDescs_.at(rasterizerName);
    }

    /// @brief ラスタライザステート追加
    /// @param rasterizerName ラスタライザ名
    /// @param rasterizerDesc ラスタライザステート
    void AddRasterizerState(const std::string &rasterizerName, D3D12_RASTERIZER_DESC rasterizerDesc) {
        rasterizerDescs_[rasterizerName] = rasterizerDesc;
    }

    /// @brief ラスタライザステート
    /// @param rasterizerName ラスタライザ名
    /// @return ラスタライザステートの参照
    [[nodiscard]] const D3D12_RASTERIZER_DESC &GetRasterizerState(const std::string &rasterizerName) const {
        return rasterizerDescs_.at(rasterizerName);
    }

    /// @brief ラスタライザステートのクリア
    void Clear() {
        rasterizerDescs_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_RASTERIZER_DESC> rasterizerDescs_;
};

} // namespace KashipanEngine