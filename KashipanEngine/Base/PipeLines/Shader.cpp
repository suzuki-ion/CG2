#include <format>
#include <cassert>
#include <d3d12shader.h>

#include "Common/ConvertString.h"
#include "Common/Logs.h"

#include "Shader.h"

namespace KashipanEngine {

namespace {

#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
)

} // namespace

Shader::Shader() {
    HRESULT hr;
    // dxcUtilsのインスタンスを生成
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    // dxcCompilerのインスタンスを生成
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    // includeHandlerのインスタンスを生成
    hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

void Shader::AddShader(const std::string &shaderName, const std::string & filePath, const std::string &profile) {
    // 文字列をUTF-16に変換
    std::wstring wFilePath = ConvertString(filePath);
    std::wstring wProfile = ConvertString(profile);
    // シェーダーをコンパイルしてIDxcBlobを取得
    IDxcBlob *shaderBlob = CompileShader(wFilePath, wProfile.c_str());
    // 取得したIDxcBlobをシェーダーキャッシュに追加
    shaderCache_[shaderName] = shaderBlob;
}

IDxcBlob *Shader::CompileShader(const std::wstring &filePath, const wchar_t *profile) {
    // これからシェーダーをコンパイルする旨をログに出力
    LogSimple(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile));

    //==================================================
    // 1. hlslファイルを読む
    //==================================================

    IDxcBlobEncoding *shaderSource = nullptr;
    HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // ファイルの読み込みに失敗した場合はエラーを出す
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer{};
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    // UTF8の文字コードであることを通知
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    //==================================================
    // 2. コンパイルする
    //==================================================

    LPCWSTR arguments[] = {
        filePath.c_str(),           // コンパイル対象のhlslファイル名
        L"-E", L"main",             // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile,             // ShaderProfileの指定
        L"-Zi", L"-Qembed_debug",   // デバッグ情報を埋め込む
        L"-Od",                     // 最適化を外しておく
        L"-Zpr",                    // メモリレイアウトは行優先
    };
    // 実際にShaderをコンパイルする
    IDxcResult *shaderResult = nullptr;
    hr = dxcCompiler_->Compile(
        &shaderSourceBuffer,        // 読み込んだファイル
        arguments,                  // コンパイルオプション
        _countof(arguments),        // コンパイルオプションの数
        includeHandler_.Get(),      // includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );
    // コンパイルエラーでなくdxcが起動できないなど致命的な状況
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    //==================================================
    // 3. 警告・エラーがでていないか確認する
    //==================================================

    // 警告・エラーが出てたらログに出して止める
    IDxcBlobUtf8 *shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if ((shaderError != nullptr) && (shaderError->GetStringLength() != 0)) {
        // エラーがあった場合はエラーを出力して終了
        LogSimple(std::format(L"Compile Failed, path:{}, profile:{}", filePath, profile), kLogLevelFlagError);
        LogSimple(shaderError->GetStringPointer(), kLogLevelFlagError);
        assert(false);
    }

    //==================================================
    // 4. コンパイル結果を受け取って返す
    //==================================================

    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob *shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    // コンパイル結果の取得に失敗した場合はエラーを出す
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // コンパイル完了のログを出力
    LogSimple(std::format(L"Compile Succeeded, path:{}, profile:{}", filePath, profile));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();

    // 実行用のバイナリを返す
    return shaderBlob;
}

} // namespace KashipanEngine