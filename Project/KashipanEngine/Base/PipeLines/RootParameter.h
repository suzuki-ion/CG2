#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

class RootParameter {
public:
    [[nodiscard]] const std::vector<D3D12_ROOT_PARAMETER> &operator[](const std::string &rootParameterName) const {
        return rootParameters_.at(rootParameterName);
    }

    /// @brief ルートパラメーター追加
    /// @param rootParameterName ルートパラメーター名
    /// @param rootParameters シェーダーのルートパラメーター
    void AddRootParameter(const std::string &rootParameterName, const std::vector<D3D12_ROOT_PARAMETER> &rootParameters) {
        rootParameters_[rootParameterName] = rootParameters;
    }

    /// @brief ルートパラメーター取得
    /// @param rootParameterName ルートパラメーター名
    /// @return ルートパラメーターの参照
    [[nodiscard]] const std::vector<D3D12_ROOT_PARAMETER> &GetRootParameter(const std::string &rootParameterName) const {
        return rootParameters_.at(rootParameterName);
    }

    /// @brief ルートパラメーターのクリア
    void Clear() {
        rootParameters_.clear();
    }

private:
    std::unordered_map<std::string, std::vector<D3D12_ROOT_PARAMETER>> rootParameters_;
};

} // namespace KashipanEngine