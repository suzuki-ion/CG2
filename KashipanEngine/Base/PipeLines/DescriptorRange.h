#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

class DescriptorRange {
public:
    [[nodiscard]] const std::vector<D3D12_DESCRIPTOR_RANGE> &operator[](const std::string &descriptorRangeName) const {
        return descriptorRanges_.at(descriptorRangeName);
    }

    /// @brief ディスクリプタレンジの追加
    /// @param descriptorRangeName ディスクリプタレンジ名
    /// @param descriptorRanges ディスクリプタレンジ
    void AddDescriptorRange(const std::string &descriptorRangeName, const std::vector<D3D12_DESCRIPTOR_RANGE> &descriptorRanges) {
        descriptorRanges_[descriptorRangeName] = descriptorRanges;
    }

    /// @brief ディスクリプタレンジの取得
    /// @param descriptorRangeName ディスクリプタレンジ名
    /// @return ディスクリプタレンジ
    [[nodiscard]] const std::vector<D3D12_DESCRIPTOR_RANGE> &GetDescriptorRange(const std::string &descriptorRangeName) const {
        return descriptorRanges_.at(descriptorRangeName);
    }

    /// @brief ディスクリプタレンジのクリア
    void Clear() {
        descriptorRanges_.clear();
    }

private:
    std::unordered_map<std::string, std::vector<D3D12_DESCRIPTOR_RANGE>> descriptorRanges_;
};

} // namespace KashipanEngine