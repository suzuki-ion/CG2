#pragma once
#include <json.hpp>
#include "PipeLines/PipeLines.h"
#include "PipeLines/ShaderReflection.h"
#include "Common/PipeLineSet.h"

namespace KashipanEngine {
using Json = nlohmann::json;

/// @brief パイプライン情報用構造体
struct PipeLineInfo {
    /// @brief パイプラインの名前
    std::string name;
    /// @brief パイプラインの種類（"Render" or "Compute"）
    std::string type;
    /// @brief コマンドリストに設定する際のトポロジータイプ
    D3D12_PRIMITIVE_TOPOLOGY topologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    /// @brief パイプラインセット
    PipeLineSet pipeLineSet;
};

class PipeLineManager {
public:
    PipeLineManager() = delete;
    /// @brief コンストラクタ
    /// @param dxCommon DirectXCommonクラスへのポインタ
    /// @param pipeLineSettingsPath パイプライン設定ファイルのパス
    PipeLineManager(
        DirectXCommon *dxCommon,
        const std::string &pipeLineSettingsPath = "Resources/PipeLineSetting.json");
    ~PipeLineManager() = default;

    /// @brief パイプラインの再読み込み
    void ReloadPipeLines();

    /// @brief パイプライン情報の取得
    /// @param pipeLineName パイプラインの名前
    /// @return PipeLineInfoの参照
    [[nodiscard]] PipeLineInfo &GetPipeLine(const std::string &pipeLineName) {
        return pipeLineInfos_.at(pipeLineName);
    }

    /// @brief パイプラインの存在確認
    /// @param pipeLineName パイプラインの名前
    /// @return 存在する場合はtrue、存在しない場合はfalse
    [[nodiscard]] bool HasPipeLine(const std::string &pipeLineName) const {
        return pipeLineInfos_.find(pipeLineName) != pipeLineInfos_.end();
    }

    /// @brief コマンドリストにパイプラインを設定
    /// @param pipeLineName 設定するパイプラインの名前
    void SetCommandListPipeLine(const std::string &pipeLineName);

private:
    /// @brief プリセットの読み込み
    void LoadPreset();
    /// @brief パイプラインの読み込み
    void LoadPipeLines();

    /// @brief レンダリングパイプラインの読み込み
    /// @param json JSONデータ
    void LoadRenderPipeLine(const Json &json);
    /// @brief コンピュートパイプラインの読み込み
    /// @param json JSONデータ
    void LoadComputePipeLine(const Json &json);

    /// @brief ルートシグネチャの読み込み
    /// @param json JSONデータ
    void LoadRootSignature(const Json &json);
    /// @brief ルートパラメーターの読み込み
    /// @param json JSONデータ
    void LoadRootParameter(const Json &json);
    /// @brief ディスクリプタレンジの読み込み
    /// @param json JSONデータ
    void LoadDescriptorRange(const Json &json);
    /// @brief ルートコンスタントの読み込み
    /// @param json JSONデータ
    void LoadRootConstants(const Json &json);
    /// @brief ルートディスクリプタの読み込み
    /// @param json JSONデータ
    void LoadRootDescriptor(const Json &json);
    /// @brief サンプラーの読み込み
    /// @param json JSONデータ
    void LoadSampler(const Json &json);
    /// @brief インプットレイアウトの読み込み
    /// @param json JSONデータ
    void LoadInputLayout(const Json &json);
    /// @brief ラスタライザステートの読み込み
    /// @param json JSONデータ
    void LoadRasterizerState(const Json &json);
    /// @brief ブレンドステートの読み込み
    /// @param json JSONデータ
    void LoadBlendState(const Json &json);
    /// @brief シェーダーの読み込み
    /// @param json JSONデータ
    void LoadShader(const Json &json);
    /// @brief 深度ステンシルステートの読み込み
    /// @param json JSONデータ
    void LoadDepthStencilState(const Json &json);
    /// @brief グラフィックスパイプラインステートの読み込み
    /// @param json JSONデータ
    void LoadGraphicsPipelineState(const Json &json);
    /// @brief コンピュートパイプラインステートの読み込み
    /// @param json JSONデータ
    void LoadComputePipelineState(const Json &json);

    /// @brief シェーダーリフレクション実行用関数
    /// @param shaderPath シェーダーのパス
    void ShaderReflectionRun(const std::string &shaderPath);

    /// @brief DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon_;

    /// @brief パイプラインの設定データ
    PipeLines pipeLines_;
    /// @brief パイプライン情報のマップ
    std::unordered_map<std::string, PipeLineInfo> pipeLineInfos_;
    /// @brief シェーダーリフレクション用クラス
    std::unique_ptr<ShaderReflection> shaderReflection_;

    /// @brief パイプライン設定ファイルのパス
    std::string pipeLineSettingsPath_;
    /// @brief パイプラインがあるフォルダのパス
    std::string pipeLineFolderPath_;
    /// @brief プリセットのフォルダ名のマップ
    std::unordered_map<std::string, std::string> presetFolderNames_;
    /// @brief 現在設定しているパイプラインの名前
    std::string currentPipeLineName_;

    /// @brief 各読み込み関数のマップ
    const std::unordered_map<std::string, std::function<void(const Json &)>> kLoadFunctions_ = {
        {"RootSignature",           [this](const Json &json) { LoadRootSignature(json); }},
        {"RootParameter",           [this](const Json &json) { LoadRootParameter(json); }},
        {"RootConstants",           [this](const Json &json) { LoadRootConstants(json); }},
        {"RootDescriptor",          [this](const Json &json) { LoadRootDescriptor(json); }},
        {"DescriptorRange",         [this](const Json &json) { LoadDescriptorRange(json); }},
        {"Sampler",                 [this](const Json &json) { LoadSampler(json); }},
        {"InputLayout",             [this](const Json &json) { LoadInputLayout(json); }},
        {"RasterizerState",         [this](const Json &json) { LoadRasterizerState(json); }},
        {"BlendState",              [this](const Json &json) { LoadBlendState(json); }},
        {"Shader",                  [this](const Json &json) { LoadShader(json); }},
        {"DepthStencilState",       [this](const Json &json) { LoadDepthStencilState(json); }},
        {"GraphicsPipelineState",   [this](const Json &json) { LoadGraphicsPipelineState(json); }},
        {"ComputePipelineState",    [this](const Json &json) { LoadComputePipelineState(json); }}
    };
};

} // namespace KashipanEngine