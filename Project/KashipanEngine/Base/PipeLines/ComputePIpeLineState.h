#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace KashipanEngine {

class ComputePipeLineState {
public:
    [[nodiscard]] const D3D12_COMPUTE_PIPELINE_STATE_DESC &operator[](const std::string &pipeLineStateName) const {
        return pipeLineStateDescs_.at(pipeLineStateName);
    }

    /// @brief パイプラインステート追加
    /// @param pipeLineStateName パイプラインステート名
    /// @param pipeLineStateDesc パイプラインステート
    void AddPipeLineState(const std::string &pipeLineStateName, D3D12_COMPUTE_PIPELINE_STATE_DESC pipeLineStateDesc) {
        pipeLineStateDescs_[pipeLineStateName] = pipeLineStateDesc;
    }

    /// @brief パイプラインステート
    /// @param pipeLineStateName パイプラインステート名
    /// @return パイプラインステートの参照
    [[nodiscard]] const D3D12_COMPUTE_PIPELINE_STATE_DESC &GetPipeLineState(const std::string &pipeLineStateName) const {
        return pipeLineStateDescs_.at(pipeLineStateName);
    }

    /// @brief パイプラインステートのクリア
    void Clear() {
        pipeLineStateDescs_.clear();
    }

private:
    std::unordered_map<std::string, D3D12_COMPUTE_PIPELINE_STATE_DESC> pipeLineStateDescs_;
};

} // namespace KashipanEngine