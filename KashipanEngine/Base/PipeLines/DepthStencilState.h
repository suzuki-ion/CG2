#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace KashipanEngine {

class DepthStencilState {
public:
    [[nodiscard]] const D3D12_DEPTH_STENCIL_DESC &operator[](const std::string &depthStencilName) const {
        return depthStencilDescs_.at(depthStencilName);
    }

    /// @brief デプスステンシル追加
    /// @param depthStencilName デプスステンシル名
    /// @param depthStencilDesc デプスステンシル
    void AddDepthStencilState(const std::string &depthStencilName, D3D12_DEPTH_STENCIL_DESC depthStencilDesc) {
        depthStencilDescs_[depthStencilName] = depthStencilDesc;
    }

    /// @brief デプスステンシル
    /// @param depthStencilName デプスステンシル名
    /// @return デプスステンシルの参照
    [[nodiscard]] const D3D12_DEPTH_STENCIL_DESC &GetDepthStencilState(const std::string &depthStencilName) const {
        return depthStencilDescs_.at(depthStencilName);
    }

    /// @brief デプスステンシルのクリア
    void Clear() {
        depthStencilDescs_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_DEPTH_STENCIL_DESC> depthStencilDescs_;
};

} // namespace KashipanEngine