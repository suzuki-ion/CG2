#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

class RootConstants {
public:
    [[nodiscard]] const D3D12_ROOT_CONSTANTS &operator[](const std::string &rootConstantName) const {
        return rootConstants_.at(rootConstantName);
    }

    /// @brief ルートコンスタント追加
    /// @param rootConstantName ルートコンスタント名
    /// @param rootConstants シェーダーのルートコンスタント
    void AddRootConstants(const std::string &rootConstantName, const D3D12_ROOT_CONSTANTS &rootConstants) {
        rootConstants_[rootConstantName] = rootConstants;
    }

    /// @brief ルートコンスタント取得
    /// @param rootConstantName ルートコンスタント名
    /// @return ルートコンスタントの参照
    [[nodiscard]] const D3D12_ROOT_CONSTANTS &GetRootConstants(const std::string &rootConstantName) const {
        return rootConstants_.at(rootConstantName);
    }

    /// @brief ルートコンスタントのクリア
    void Clear() {
        rootConstants_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_ROOT_CONSTANTS> rootConstants_;
};

} // namespace KashipanEngine