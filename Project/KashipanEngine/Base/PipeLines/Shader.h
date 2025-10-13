#pragma once
#include <wrl.h>
#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <dxcapi.h>

namespace KashipanEngine {

class Shader {
public:
    [[nodiscard]] IDxcBlob *operator[](const std::string &filePath) {
        return GetShader(filePath);
    }

    Shader();
    ~Shader() = default;

    /// @brief シェーダー追加
    /// @param shaderName シェーダーの名前（キャッシュ用）
    /// @param filePath シェーダーファイルへのパス
    /// @param profile コンパイルに使用するプロファイル（例："vs_6_0"、"ps_6_0"など）
    void AddShader(const std::string &shaderName, const std::string &filePath, const std::string &profile);

    /// @brief シェーダー取得
    /// @param shaderName シェーダーの名前（キャッシュ用）
    /// @return コンパイル済みシェーダーのBlobポインタ
    [[nodiscard]] IDxcBlob *GetShader(const std::string &shaderName) {
        auto it = shaderCache_.find(shaderName);
        if (it != shaderCache_.end()) {
            return it->second.Get();
        }
        return nullptr;
    }

    /// @brief シェーダーキャッシュをクリア
    void Clear() { shaderCache_.clear(); }

private:
    /// @brief シェーダーコンパイル用関数
    /// @param filePath コンパイルするシェーダーファイルへのパス
    /// @param profile コンパイルに使用するプロファイル
    /// @return コンパイル結果
    IDxcBlob *CompileShader(const std::wstring &filePath, const wchar_t *profile);

    /// @brief IDxcUtilsインターフェース
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
    /// @brief IDxcCompiler3インターフェース
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
    /// @brief IDxcIncludeHandlerインターフェース
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

    /// @brief コンパイル済みシェーダーのキャッシュ
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;
};

} // namespace KashipanEngine