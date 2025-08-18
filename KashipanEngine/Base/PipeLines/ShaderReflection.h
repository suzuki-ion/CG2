#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <dxcapi.h>

namespace KashipanEngine {

struct ShaderReflectionInfo {
    /// @brief シェーダーのリフレクション情報
    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderReflection;
    /// @brief ルートパラメータ
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    /// @brief インプットレイアウト
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
};

class ShaderReflection {
public:
    ShaderReflection() = delete;
    /// @brief コンストラクタ
    /// @param device D3D12デバイスへのポインタ
    ShaderReflection(ID3D12Device *device);
    ~ShaderReflection() = default;

    /// @brief シェーダーの実行用バイナリからリフレクションを取得
    /// @param shaderBlob シェーダーの実行用バイナリ
    /// @return シェーダーのリフレクション情報
    [[nodiscard]] ShaderReflectionInfo GetShaderReflection(IDxcBlob *shaderBlob) const;

private:
    /// @brief ルートパラメータの作成
    /// @param shaderReflectionInfo シェーダーのリフレクション情報
    /// @return 作成されたルートパラメータの配列
    const std::vector<D3D12_ROOT_PARAMETER> &CreateRootParameters(
        ID3D12ShaderReflection *shaderReflectionInfo) const;

    /// @brief インプットレイアウトの作成
    /// @param shaderReflectionInfo シェーダーのリフレクション情報
    /// @return 作成されたインプットレイアウトの配列
    const std::vector<D3D12_INPUT_ELEMENT_DESC> &CreateInputLayout(
        ID3D12ShaderReflection *shaderReflectionInfo) const;

    // D3D12デバイスへのポインタ
    ID3D12Device *device_;
    Microsoft::WRL::ComPtr<IDxcLibrary> dxcLibrary_;
};

} // namespace KashipanEngine