#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace KashipanEngine {

struct InputLayoutStruct {
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
};

class InputLayout {
public:
    [[nodiscard]] const D3D12_INPUT_LAYOUT_DESC &operator[](const std::string &inputLayoutName) const {
        return GetInputLayout(inputLayoutName);
    }

    /// @brief インプットレイアウトの追加
    /// @param inputLayoutName インプットレイアウト名
    /// @param descriptorRanges インプットレイアウト
    void AddInputLayout(const std::string &inputLayoutName, const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputElementDescs) {
        inputLayouts_[inputLayoutName].inputElementDescs = inputElementDescs;
        inputLayouts_[inputLayoutName].inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs.size());
        inputLayouts_[inputLayoutName].inputLayoutDesc.pInputElementDescs = inputLayouts_[inputLayoutName].inputElementDescs.data();
    }

    /// @brief インプットレイアウトの取得
    /// @param inputLayoutName インプットレイアウト名
    /// @return インプットレイアウト
    [[nodiscard]] const D3D12_INPUT_LAYOUT_DESC &GetInputLayout(const std::string &inputLayoutName) const {
        return inputLayouts_.at(inputLayoutName).inputLayoutDesc;
    }

    /// @brief インプットレイアウトのクリア
    void Clear() {
        inputLayouts_.clear();
    }

private:
    std::unordered_map<std::string, InputLayoutStruct> inputLayouts_;
};

} // namespace KashipanEngine