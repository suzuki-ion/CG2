#pragma once
#include "PipeLines/PipeLines.h"
#include "Common/PipeLineSet.h"

namespace KashipanEngine {

class PipeLineManager {
public:
    PipeLineManager(
        DirectXCommon *dxCommon,
        const std::string &pipeLineSettingsPath = "Resources/PipeLineSetting.json");
    ~PipeLineManager() = default;

    /// @brief パイプラインの取得
    /// @param pipeLineName パイプラインの名前
    /// @return PipeLineの参照
    [[nodiscard]] PipeLineSet &GetPipeLine(const std::string &pipeLineName) {
        return pipeLineSets_.at(pipeLineName);
    }

private:
    /// @brief プリセットの読み込み
    void LoadPreset();
    /// @brief ルートシグネチャのプリセットを読み込む
    void LoadRootSignaturePreset();
    /// @brief ルートパラメーターのプリセットを読み込む
    void LoadRootParameterPreset();
    /// @brief ディスクリプタレンジのプリセットを読み込む
    void LoadDescriptorRangePreset();
    /// @brief サンプラーのプリセットを読み込む
    void LoadSamplerPreset();
    /// @brief インプットレイアウトのプリセットを読み込む
    void LoadInputLayoutPreset();
    /// @brief ラスタライザステートのプリセットを読み込む
    void LoadRasterizerStatePreset();
    /// @brief ブレンドステートのプリセットを読み込む
    void LoadBlendStatePreset();
    /// @brief シェーダーのプリセットを読み込む
    void LoadShaderPreset();
    /// @brief 深度ステンシルステートのプリセットを読み込む
    void LoadDepthStencilStatePreset();

    /// @brief Jsonファイルからパイプラインを読み込む
    /// @param jsonPath 読み込むJsonファイルのパス
    void LoadPipeLine4Json(const std::string &jsonPath);
    
    /// @brief DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon_;
    PipeLines pipeLines_;
    std::unordered_map<std::string, PipeLineSet> pipeLineSets_;

    /// @brief 各プリセット読み込み関数のマップ
    std::unordered_map<std::string, std::function<void()>> presetLoadFunctions_ = {
        {"RootSignature",       [this]() { LoadRootSignaturePreset(); }},
        {"RootParameter",       [this]() { LoadRootParameterPreset(); }},
        {"DescriptorRange",     [this]() { LoadDescriptorRangePreset(); }},
        {"Sampler",             [this]() { LoadSamplerPreset(); }},
        {"InputLayout",         [this]() { LoadInputLayoutPreset(); }},
        {"RasterizerState",     [this]() { LoadRasterizerStatePreset(); }},
        {"BlendState",          [this]() { LoadBlendStatePreset(); }},
        {"Shader",              [this]() { LoadShaderPreset(); }},
        {"DepthStencilState",   [this]() { LoadDepthStencilStatePreset(); }}
    };
};

} // namespace KashipanEngine