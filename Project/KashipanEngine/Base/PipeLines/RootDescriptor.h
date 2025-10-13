#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

class RootDescriptor {
public:
    [[nodiscard]] const D3D12_ROOT_DESCRIPTOR &operator[](const std::string &rootDescriptorName) const {
        return rootDescriptors_.at(rootDescriptorName);
    }

    /// @brief ルートディスクリプタ追加
    /// @param rootDescriptorName ルートディスクリプタ名
    /// @param rootDescriptors シェーダーのルートディスクリプタ
    void AddRootDescriptor(const std::string &rootDescriptorName, const D3D12_ROOT_DESCRIPTOR &rootDescriptors) {
        rootDescriptors_[rootDescriptorName] = rootDescriptors;
    }

    /// @brief ルートディスクリプタ取得
    /// @param rootDescriptorName ルートディスクリプタ名
    /// @return ルートディスクリプタの参照
    [[nodiscard]] const D3D12_ROOT_DESCRIPTOR &GetRootDescriptor(const std::string &rootDescriptorName) const {
        return rootDescriptors_.at(rootDescriptorName);
    }

    /// @brief ルートディスクリプタのクリア
    void Clear() {
        rootDescriptors_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_ROOT_DESCRIPTOR> rootDescriptors_;
};

} // namespace KashipanEngine