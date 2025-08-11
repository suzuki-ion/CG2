#pragma once
#include <wrl.h>
#include <d3d12shader.h>
#include <dxcapi.h>

namespace KashipanEngine {

class ShaderReflection {
public:
    ShaderReflection();
    ~ShaderReflection() = default;

    /// @brief シェーダーの実行用バイナリからリフレクションを取得
    /// @param shaderBlob シェーダーの実行用バイナリ
    /// @return ID3D12ShaderReflectionインターフェースへのポインタ
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12ShaderReflection> GetShaderReflection(IDxcBlob *shaderBlob) const;

private:
    Microsoft::WRL::ComPtr<IDxcLibrary> dxcLibrary_;
};

} // namespace KashipanEngine