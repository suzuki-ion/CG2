#include "DirectXCommon.h"
#include "Common/JsoncLoader.h"
#include "Common/DirectoryLoader.h"
#include "Common/Logs.h"
#include "PipeLines/EnumMaps.h"
#include "PipeLines/DefineMaps.h"
#include "PipeLineManager.h"

namespace KashipanEngine {
using namespace KashipanEngine::PipeLine::EnumMaps;
using namespace KashipanEngine::PipeLine::DefineMaps;

PipeLineManager::PipeLineManager(DirectXCommon *dxCommon, const std::string &pipeLineSettingsPath) {
    Log("PipeLineManager constructor called.");
    if (!dxCommon) {
        LogSimple("DirectXCommon pointer is null.", kLogLevelFlagError);
        assert(false);
    }
    dxCommon_ = dxCommon;
    shaderReflection_ = std::make_unique<ShaderReflection>(dxCommon_->GetDevice());

    LogSimple("Loading PipeLine Presets.");
    pipeLineSettingsPath_ = pipeLineSettingsPath;
    Json pipeLineSettings = LoadJsonc(pipeLineSettingsPath);
    
    pipeLineFolderPath_ = pipeLineSettings["PipeLineFolder"].get<std::string>();
    presetFolderNames_ = pipeLineSettings["PresetFolders"].get<std::unordered_map<std::string, std::string>>();
    LoadPreset();
    LogSimple("PipeLine Presets loaded.");
    LogSimple("Loading PipeLines.");
    LoadPipeLines();
    LogSimple("PipeLines loaded successfully.");
}

void PipeLineManager::ReloadPipeLines() {
    Log("Reloading PipeLines.");
    pipeLines_.Reset();
    LoadPreset();
    LogSimple("PipeLines reloaded from presets.");
    LoadPipeLines();
    LogSimple("PipeLines reloaded successfully.");
}

void PipeLineManager::SetCommandListPipeLine(const std::string &pipeLineName) {
    Log("Setting command list pipeline: " + pipeLineName);
    auto it = pipeLineSets_.find(pipeLineName);
    if (it != pipeLineSets_.end()) {
        auto &pipeLineSet = it->second;
        dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipeLineSet.rootSignature.Get());
        dxCommon_->GetCommandList()->SetPipelineState(pipeLineSet.pipelineState.Get());
        LogSimple("Command list pipeline set to: " + pipeLineName, kLogLevelFlagInfo);
    } else {
        LogSimple("PipeLine not found: " + pipeLineName, kLogLevelFlagError);
    }
}

void PipeLineManager::LoadPreset() {
    for (const auto &presetFolder : presetFolderNames_) {
        auto presetFiles = GetFilesInDirectory(presetFolder.second, {".json", ".jsonc"}, false);
        for (const auto &file : presetFiles) {
            kLoadFunctions_.at(presetFolder.first)(LoadJsonc(file));
        }
    }
}

void PipeLineManager::LoadPipeLines() {
    auto pipeLineFiles = GetFilesInDirectory(pipeLineFolderPath_, {".json", ".jsonc"}, false);
    for (const auto &file : pipeLineFiles) {
        Json pipeLineJson = LoadJsonc(file);
        // PipeLineの種類を判定してロード
        if (pipeLineJson.contains("PipeLineType")) {
            std::string type = pipeLineJson["PipeLineType"].get<std::string>();
            if (type == "Render") {
                LoadRenderPipeLine(pipeLineJson);
            } else if (type == "Compute") {
                LoadComputePipeLine(pipeLineJson);
            } else {
                LogSimple("Unknown PipeLine type: " + type, kLogLevelFlagWarning);
            }
        } else {
            LogSimple("PipeLine JSON is missing 'Type' field.", kLogLevelFlagWarning);
        }
    }
}

void PipeLineManager::LoadRenderPipeLine(const Json &json) {
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
        LogSimple("Loading PipeLine: " + name, kLogLevelFlagInfo);
    } else {
        LogSimple("PipeLine JSON is missing 'Name' field.", kLogLevelFlagWarning);
    }

    HRESULT hr;
    std::unordered_map<std::string, IDxcBlob *> shaders = {
            { "Vertex",     nullptr },
            { "Pixel",      nullptr },
            { "Geometry",   nullptr },
            { "Hull",       nullptr },
            { "Domain",     nullptr }
    };
    std::vector<std::string> shadersOrder = { "Vertex", "Pixel", "Geometry", "Hull", "Domain" };
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    D3D12_BLEND_DESC blendDesc = {};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineDesc = {};

    LogSimple("PipeLine JSON loaded: " + name, kLogLevelFlagInfo);

    if (json.contains("Shader")) {
        Json shadersJson = json["Shader"];
        // 頂点シェーダーだけは必須なので、存在しない場合は警告を出して処理を中断
        if (!shadersJson.contains("Vertex")) {
            LogSimple("VertexShader is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
            return;
        }

        for (const auto &shaderType : shadersOrder) {
            if (!shadersJson.contains(shaderType)) {
                continue;
            }
            Json shaderJson = shadersJson[shaderType];
            std::string shaderName;
            if (shaderJson.contains("UsePreset")) {
                std::string presetName = shaderJson["UsePreset"].get<std::string>();
                shaders[shaderType] = pipeLines_.shader->GetShader(presetName);
                if (!shaders[shaderType]) {
                    LogSimple("Shader preset not found: " + presetName, kLogLevelFlagWarning);
                }
                shaderName = presetName;
            } else {
                // シェーダー名をPipeLine名に設定
                shaderJson["Name"] = name + "_" + shaderType;
                // 種類を設定
                shaderJson["Type"] = shaderType;
                LogSimple("Shader name not found in JSON, using default: " + shaderJson["Name"].get<std::string>(), kLogLevelFlagInfo);
                LoadShader(shaderJson);
                shaders[shaderType] = pipeLines_.shader->GetShader(shaderJson["Name"].get<std::string>());
                if (!shaders[shaderType]) {
                    LogSimple("Shader not found: " + shaderJson["Name"].get<std::string>(), kLogLevelFlagWarning);
                }
                shaderName = shaderJson["Name"].get<std::string>();
            }
            // シェーダー名からルートパラメーターを取得
            auto rootParametersForShader = pipeLines_.rootParameter->GetRootParameter(shaderName);
            rootParameters.insert(rootParameters.end(), rootParametersForShader.begin(), rootParametersForShader.end());

            // 頂点シェーダーの場合は入力レイアウトも取得
            if (shaderType == "Vertex") {
                inputLayoutDesc = pipeLines_.inputLayout->GetInputLayout(shaderName);
            }
        }
    }

    if (json.contains("RootSignature")) {
        Json rootSignatureJson = json["RootSignature"];
        if (rootSignatureJson.contains("UsePreset")) {
            std::string presetName = rootSignatureJson["UsePreset"].get<std::string>();
            rootSignatureDesc = pipeLines_.rootSignature->GetRootSignatureDesc(presetName);
        } else {
            // ルートシグネチャの名前をPipeLine名に設定
            rootSignatureJson["Name"] = name;
            LoadRootSignature(rootSignatureJson);
            rootSignatureDesc = pipeLines_.rootSignature->GetRootSignatureDesc(name);
        }
        // ルートシグネチャにデフォルトで設定されているか、
        // 或いはルートシグネチャの設定にルートパラメーターが含まれている場合はそっちのルートパラメーターを優先
        if ((rootSignatureDesc.NumParameters > 0) && (rootSignatureDesc.pParameters != nullptr)) {
            // クリアすればこの後の生成で上書きされずに済むのでここでクリア
            rootParameters.clear();
        }
        if ((rootSignatureDesc.NumStaticSamplers > 0) && (rootSignatureDesc.pStaticSamplers != nullptr)) {
            // クリアすればこの後の生成で上書きされずに済むのでここでクリア
            samplers.clear();
        }
        // 記述がある場合はそちらを優先
        if (rootSignatureJson.contains("RootParameter")) {
            // ルートシグネチャのルートパラメーターを優先するため、既存のルートパラメーターをクリア
            rootParameters.clear();
            Json rootParameterJson = rootSignatureJson["RootParameter"];
            if (rootParameterJson.contains("UsePreset")) {
                auto presetRootParameters = pipeLines_.rootParameter->GetRootParameter(rootParameterJson["UsePreset"].get<std::string>());
                rootParameters.insert(rootParameters.end(), presetRootParameters.begin(), presetRootParameters.end());
            } else {
                auto customRootParameters = pipeLines_.rootParameter->GetRootParameter(name);
                rootParameters.insert(rootParameters.end(), customRootParameters.begin(), customRootParameters.end());
            }
        }
        if (rootSignatureJson.contains("Sampler")) {
            Json samplerJson = rootSignatureJson["Sampler"];
            if (samplerJson.contains("UsePreset")) {
                auto presetSamplers = pipeLines_.sampler->GetSampler(samplerJson["UsePreset"].get<std::string>());
                samplers.insert(samplers.end(), presetSamplers.begin(), presetSamplers.end());
            } else {
                auto customSamplers = pipeLines_.sampler->GetSampler(name);
                samplers.insert(samplers.end(), customSamplers.begin(), customSamplers.end());
            }
        }
    } else {
        LogSimple("RootSignature is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    // ルートシグネチャの作成
    if (!rootParameters.empty()) {
        rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
        rootSignatureDesc.pParameters = rootParameters.data();
    }
    if (!samplers.empty()) {
        rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(samplers.size());
        rootSignatureDesc.pStaticSamplers = samplers.data();
    }
    
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    hr = D3D12SerializeRootSignature(
        &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr)) {
        if (errorBlob) {
            LogSimple("Root signature serialization failed: " + std::string(static_cast<const char*>(errorBlob->GetBufferPointer())), kLogLevelFlagError);
        } else {
            LogSimple("Root signature serialization failed with unknown error.", kLogLevelFlagError);
        }
        return;
    }
    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) {
        LogSimple("Failed to create root signature: " + name, kLogLevelFlagError);
        return;
    }

    // 入力レイアウトの設定
    if (json.contains("InputLayout")) {
        Json inputLayoutJson = json["InputLayout"];
        if (inputLayoutJson.contains("UsePreset")) {
            inputLayoutDesc = pipeLines_.inputLayout->GetInputLayout(inputLayoutJson["UsePreset"].get<std::string>());
        } else {
            // 入力レイアウトの名前をPipeLine名に設定
            inputLayoutJson["Name"] = name;
            LoadInputLayout(inputLayoutJson);
            inputLayoutDesc = pipeLines_.inputLayout->GetInputLayout(name);
        }
    }

    // ラスタライザーステートの設定
    if (json.contains("RasterizerState")) {
        Json rasterizerJson = json["RasterizerState"];
        if (rasterizerJson.contains("UsePreset")) {
            rasterizerDesc = pipeLines_.rasterizerState->GetRasterizerState(rasterizerJson["UsePreset"].get<std::string>());
        } else {
            // ラスタライザーステートの名前をPipeLine名に設定
            rasterizerJson["Name"] = name;
            LoadRasterizerState(rasterizerJson);
            rasterizerDesc = pipeLines_.rasterizerState->GetRasterizerState(name);
        }
    } else {
        LogSimple("Rasterizer state is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    // ブレンドステートの設定
    if (json.contains("BlendState")) {
        Json blendJson = json["BlendState"];
        if (blendJson.contains("UsePreset")) {
            blendDesc = pipeLines_.blendState->GetBlendState(blendJson["UsePreset"].get<std::string>());
        } else {
            // ブレンドステートの名前をPipeLine名に設定
            blendJson["Name"] = name;
            LoadBlendState(blendJson);
            blendDesc = pipeLines_.blendState->GetBlendState(name);
        }
    } else {
        LogSimple("Blend state is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    // デプスステンシルステートの設定
    if (json.contains("DepthStencilState")) {
        Json depthStencilJson = json["DepthStencilState"];
        if (depthStencilJson.contains("UsePreset")) {
            depthStencilDesc = pipeLines_.depthStencilState->GetDepthStencilState(depthStencilJson["UsePreset"].get<std::string>());
        } else {
            // デプスステンシルステートの名前をPipeLine名に設定
            depthStencilJson["Name"] = name;
            LoadDepthStencilState(depthStencilJson);
            depthStencilDesc = pipeLines_.depthStencilState->GetDepthStencilState(name);
        }
    } else {
        LogSimple("Depth stencil state is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    // パイプラインステートの設定
    if (json.contains("PipelineState")) {
        Json pipeLineStateJson = json["PipelineState"];
        if (pipeLineStateJson.contains("UsePreset")) {
            pipeLineDesc = pipeLines_.graphicsPipeLineState->GetPipeLineState(pipeLineStateJson["UsePreset"].get<std::string>());
        } else {
            // パイプラインステートの名前をPipeLine名に設定
            pipeLineStateJson["Name"] = name;
            LoadGraphicsPipelineState(pipeLineStateJson);
            pipeLineDesc = pipeLines_.graphicsPipeLineState->GetPipeLineState(name);
        }
    } else {
        LogSimple("Pipeline state is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    pipeLineDesc.pRootSignature = rootSignature.Get();
    if (shaders["Vertex"]) {
        pipeLineDesc.VS = { shaders["Vertex"]->GetBufferPointer(), shaders["Vertex"]->GetBufferSize() };
    }
    if (shaders["Pixel"]) {
        pipeLineDesc.PS = { shaders["Pixel"]->GetBufferPointer(), shaders["Pixel"]->GetBufferSize() };
    }
    if (shaders["Geometryr"]) {
        pipeLineDesc.GS = { shaders["Geometry"]->GetBufferPointer(), shaders["Geometry"]->GetBufferSize() };
    }
    if (shaders["Hull"]) {
        pipeLineDesc.HS = { shaders["Hull"]->GetBufferPointer(), shaders["Hull"]->GetBufferSize() };
    }
    if (shaders["Domain"]) {
        pipeLineDesc.DS = { shaders["Domain"]->GetBufferPointer(), shaders["Domain"]->GetBufferSize() };
    }
    pipeLineDesc.InputLayout = inputLayoutDesc;
    pipeLineDesc.RasterizerState = rasterizerDesc;
    pipeLineDesc.BlendState = blendDesc;
    pipeLineDesc.DepthStencilState = depthStencilDesc;
    
    // パイプライン生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
        &pipeLineDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) {
        LogSimple("Failed to create graphics pipeline state: " + name, kLogLevelFlagError);
        return;
    }

    // 生成したものをセットにする
    PipeLineSet pipeLineSet;
    pipeLineSet.rootSignature = rootSignature;
    pipeLineSet.pipelineState = pipelineState;
    // セットにしたものをマップに登録
    pipeLineSets_[name] = pipeLineSet;

    LogSimple("Graphics pipeline created successfully: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadComputePipeLine(const Json &json) {
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
        LogSimple("Loading Compute PipeLine: " + name, kLogLevelFlagInfo);
    } else {
        LogSimple("Compute PipeLine JSON is missing 'Name' field.", kLogLevelFlagWarning);
    }

    HRESULT hr;
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipeLineDesc = {};
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    IDxcBlob *computeShader = nullptr;
    
    if (json.contains("Shader")) {
        Json shaderJson = json["Shader"];
        std::string shaderName;
        if (shaderJson.contains("UsePreset")) {
            computeShader = pipeLines_.shader->GetShader(shaderJson["UsePreset"].get<std::string>());
            if (!computeShader) {
                LogSimple("Compute shader preset not found: " + shaderJson["UsePreset"].get<std::string>(), kLogLevelFlagWarning);
            }
            shaderName = shaderJson["UsePreset"].get<std::string>();

        } else {
            // コンピュートシェーダーの名前をPipeLine名に設定
            shaderJson["Name"] = name + "_Compute";
            LoadShader(shaderJson);
            computeShader = pipeLines_.shader->GetShader(shaderJson["Name"].get<std::string>());
            if (!computeShader) {
                LogSimple("Compute shader not found: " + shaderJson["Name"].get<std::string>(), kLogLevelFlagWarning);
            }
            shaderName = shaderJson["Name"].get<std::string>();
        }
        // シェーダー名からルートパラメーターを取得
        rootParameters = pipeLines_.rootParameter->GetRootParameter(shaderName);

    } else {
        LogSimple("Compute shader is required but not found in Compute PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    if (json.contains("RootSignature")) {
        Json rootSignatureJson = json["RootSignature"];
        if (rootSignatureJson.contains("UsePreset")) {
            std::string presetName = rootSignatureJson["UsePreset"].get<std::string>();
            rootSignatureDesc = pipeLines_.rootSignature->GetRootSignatureDesc(presetName);
        } else {
            // ルートシグネチャの名前をPipeLine名に設定
            rootSignatureJson["Name"] = name;
            LoadRootSignature(rootSignatureJson);
            rootSignatureDesc = pipeLines_.rootSignature->GetRootSignatureDesc(name);
        }
        // ルートシグネチャの設定にルートパラメーターが含まれている場合はそっちのルートパラメーターを優先
        if (rootSignatureJson.contains("RootParameter")) {
            // ルートシグネチャのルートパラメーターを優先するため、既存のルートパラメーターをクリア
            rootParameters.clear();
            Json rootParameterJson = rootSignatureJson["RootParameter"];
            if (rootParameterJson.contains("UsePreset")) {
                auto presetRootParameters = pipeLines_.rootParameter->GetRootParameter(rootParameterJson["UsePreset"].get<std::string>());
                rootParameters.insert(rootParameters.end(), presetRootParameters.begin(), presetRootParameters.end());
            } else {
                auto customRootParameters = pipeLines_.rootParameter->GetRootParameter(name);
                rootParameters.insert(rootParameters.end(), customRootParameters.begin(), customRootParameters.end());
            }
        }
    } else {
        LogSimple("RootSignature is required but not found in PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    // ルートシグネチャの作成
    rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
    rootSignatureDesc.pParameters = rootParameters.data();
    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0, &rootSignatureDesc, sizeof(rootSignatureDesc), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) {
        LogSimple("Failed to create root signature: " + name, kLogLevelFlagError);
        return;
    }

    // コンピュートパイプラインステートの設定
    if (json.contains("PipelineState")) {
        Json pipeLineStateJson = json["PipelineState"];
        if (pipeLineStateJson.contains("UsePreset")) {
            computePipeLineDesc = pipeLines_.computePipeLineState->GetPipeLineState(pipeLineStateJson["UsePreset"].get<std::string>());
        } else {
            // パイプラインステートの名前をPipeLine名に設定
            pipeLineStateJson["Name"] = name;
            LoadComputePipelineState(pipeLineStateJson);
            computePipeLineDesc = pipeLines_.computePipeLineState->GetPipeLineState(name);
        }
    } else {
        LogSimple("Pipeline state is required but not found in Compute PipeLine JSON.", kLogLevelFlagWarning);
        return;
    }

    computePipeLineDesc.pRootSignature = rootSignature.Get();
    computePipeLineDesc.CS = { computeShader->GetBufferPointer(), computeShader->GetBufferSize() };

    // パイプライン生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
    hr = dxCommon_->GetDevice()->CreateComputePipelineState(
        &computePipeLineDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) {
        LogSimple("Failed to create compute pipeline state: " + name, kLogLevelFlagError);
        return;
    }

    // 生成したものをセットにする
    PipeLineSet pipeLineSet;
    pipeLineSet.rootSignature = rootSignature;
    pipeLineSet.pipelineState = pipelineState;
    // セットにしたものをマップに登録
    pipeLineSets_[name] = pipeLineSet;

    LogSimple("Compute pipeline state created successfully: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadRootSignature(const Json &json) {
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Root signature name is missing.", kLogLevelFlagWarning);
        return;
    }

    LogSimple("Loading root signature: " + name, kLogLevelFlagInfo);
    if (json.contains("Flags")) {
        rootSignatureDesc.Flags = kRootSignatureFlagsMap.at(json["Flags"].get<std::string>());
    }
    if (json.contains("RootParameter")) {
        Json rootParameterJson = json["RootParameter"];
        std::string rootParameterName;
        if (rootParameterJson.contains("UsePreset")) {
            rootParameterName = rootParameterJson["UsePreset"].get<std::string>();
            
        } else {
            // ルートパラメーター名をルートシグネチャ名に設定
            rootParameterJson["Name"] = name;
            LoadRootParameter(rootParameterJson);
            rootParameterName = name;
        }
        const auto &rootParameters = pipeLines_.rootParameter->GetRootParameter(rootParameterName);
        rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
        rootSignatureDesc.pParameters = rootParameters.data();
    }
    if (json.contains("Sampler")) {
        Json samplerJson = json["Sampler"];
        std::string samplerName;
        if (samplerJson.contains("UsePreset")) {
            samplerName = samplerJson["UsePreset"].get<std::string>();

        } else {
            // サンプラー名をルートシグネチャ名に設定
            samplerJson["Name"] = name;
            LoadSampler(samplerJson);
            samplerName = name;
        }
        const auto &samplers = pipeLines_.sampler->GetSampler(samplerName);
        rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(samplers.size());
        rootSignatureDesc.pStaticSamplers = samplers.data();
    }

    pipeLines_.rootSignature->AddRootSignature(name, rootSignatureDesc);
    LogSimple("Root signature loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadRootParameter(const Json &json) {
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Root parameter name is missing.", kLogLevelFlagWarning);
        return;
    }

    LogSimple("Loading root parameter: " + name, kLogLevelFlagInfo);
    UINT i = 0;
    for (const auto &param : json["Parameters"]) {
        D3D12_ROOT_PARAMETER rootParam = {};
        if (param.contains("ParameterType")) {
            rootParam.ParameterType = kRootParameterTypeMap.at(param["ParameterType"].get<std::string>());
        } else {
            LogSimple("Root parameter type is missing.", kLogLevelFlagWarning);
            continue;
        }
        if (param.contains("ShaderVisibility")) {
            rootParam.ShaderVisibility = kShaderVisibilityMap.at(param["ShaderVisibility"].get<std::string>());
        } else {
            LogSimple("Root parameter shader visibility is missing.", kLogLevelFlagWarning);
            continue;
        }
        if (param.contains("DescriptorTable")) {
            Json descriptorTableJson = param["DescriptorTable"];
            // 名前をルートパラメーター名 + インデックスに設定
            descriptorTableJson["Name"] = name + "_" + std::to_string(i++);
            LoadDescriptorRange(descriptorTableJson);
            const auto &descriptorRanges = pipeLines_.descriptorRange->GetDescriptorRange(descriptorTableJson["Name"].get<std::string>());
            rootParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRanges.size());
            rootParam.DescriptorTable.pDescriptorRanges = descriptorRanges.data();

        } else if (param.contains("Constants")) {
            Json rootConstantsJson = param["Constants"];
            // 名前をルートパラメーター名 + インデックスに設定
            rootConstantsJson["Name"] = name + "_" + std::to_string(i++);
            LoadRootConstants(rootConstantsJson);
            rootParam.Constants = pipeLines_.rootConstants->GetRootConstants(rootConstantsJson["Name"].get<std::string>());
        
        } else if (param.contains("Descriptor")) {
            Json rootDescriptorJson = param["Descriptor"];
            // 名前をルートパラメーター名 + インデックスに設定
            rootDescriptorJson["Name"] = name + "_" + std::to_string(i++);
            LoadRootDescriptor(rootDescriptorJson);
            rootParam.Descriptor = pipeLines_.rootDescriptor->GetRootDescriptor(rootDescriptorJson["Name"].get<std::string>());
        }
        rootParameters.push_back(rootParam);
    }

    pipeLines_.rootParameter->AddRootParameter(name, rootParameters);
    LogSimple("Root parameter loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadDescriptorRange(const Json &json) {
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;

    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Descriptor range name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading descriptor range: " + name, kLogLevelFlagInfo);
    for (const auto &range : json["Ranges"]) {
        D3D12_DESCRIPTOR_RANGE descriptorRange = {};
        if (range.contains("RangeType")) {
            descriptorRange.RangeType = kDescriptorRangeTypeMap.at(range["RangeType"].get<std::string>());
        } else {
            LogSimple("Descriptor range type is missing.", kLogLevelFlagWarning);
            continue;
        }
        if (range.contains("NumDescriptors")) {
            descriptorRange.NumDescriptors = range["NumDescriptors"].get<UINT>();
        }
        if (range.contains("BaseShaderRegister")) {
            descriptorRange.BaseShaderRegister = range["BaseShaderRegister"].get<UINT>();
        }
        if (range.contains("RegisterSpace")) {
            descriptorRange.RegisterSpace = range["RegisterSpace"].get<UINT>();
        }
        if (range.contains("OffsetInDescriptorsFromTableStart")) {
            // 型が文字列の場合はマップから取得
            if (range["OffsetInDescriptorsFromTableStart"].is_string()) {
                descriptorRange.OffsetInDescriptorsFromTableStart = std::get<UINT>(kDefineMap.at(range["OffsetInDescriptorsFromTableStart"].get<std::string>()));
            } else if (range["OffsetInDescriptorsFromTableStart"].is_number()) {
                // 数値の場合はそのまま使用
                descriptorRange.OffsetInDescriptorsFromTableStart = range["OffsetInDescriptorsFromTableStart"].get<UINT>();
            }
        }
        descriptorRanges.push_back(descriptorRange);
    }

    pipeLines_.descriptorRange->AddDescriptorRange(name, descriptorRanges);
    LogSimple("Descriptor range loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadRootConstants(const Json &json) {
    D3D12_ROOT_CONSTANTS rootConstants = {};
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Root constants name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading root constants: " + name, kLogLevelFlagInfo);
    if (json.contains("ShaderRegister")) {
        rootConstants.ShaderRegister = json["ShaderRegister"].get<UINT>();
    }
    if (json.contains("RegisterSpace")) {
        rootConstants.RegisterSpace = json["RegisterSpace"].get<UINT>();
    }
    if (json.contains("Num32BitValues")) {
        rootConstants.Num32BitValues = json["Num32BitValues"].get<UINT>();
    }
    
    pipeLines_.rootConstants->AddRootConstants(name, rootConstants);
    LogSimple("Root constants loaded: " + name, kLogLevelFlagInfo); 
}

void PipeLineManager::LoadRootDescriptor(const Json &json) {
    D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Root descriptor name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading root descriptor: " + name, kLogLevelFlagInfo);
    if (json.contains("ShaderRegister")) {
        rootDescriptor.ShaderRegister = json["ShaderRegister"].get<UINT>();
    }
    if (json.contains("RegisterSpace")) {
        rootDescriptor.RegisterSpace = json["RegisterSpace"].get<UINT>();
    }
    
    pipeLines_.rootDescriptor->AddRootDescriptor(name, rootDescriptor);
    LogSimple("Root descriptor loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadSampler(const Json &json) {
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Sampler name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading sampler: " + name, kLogLevelFlagInfo);
    for (const auto &sampler : json["Samplers"]) {
        D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
        if (sampler.contains("Filter")) {
            samplerDesc.Filter = kFilterMap.at(sampler["Filter"].get<std::string>());
        }
        if (sampler.contains("AddressU")) {
            samplerDesc.AddressU = kTextureAddressModeMap.at(sampler["AddressU"].get<std::string>());
        }
        if (sampler.contains("AddressV")) {
            samplerDesc.AddressV = kTextureAddressModeMap.at(sampler["AddressV"].get<std::string>());
        }
        if (sampler.contains("AddressW")) {
            samplerDesc.AddressW = kTextureAddressModeMap.at(sampler["AddressW"].get<std::string>());
        }
        if (sampler.contains("MipLODBias")) {
            if (sampler["MipLODBias"].is_string()) {
                samplerDesc.MipLODBias = std::get<float>(kDefineMap.at(sampler["MipLODBias"].get<std::string>()));
            } else if (sampler["MipLODBias"].is_number()) {
                samplerDesc.MipLODBias = sampler["MipLODBias"].get<float>();
            }
        }
        if (sampler.contains("MaxAnisotropy")) {
            if (sampler["MaxAnisotropy"].is_string()) {
                samplerDesc.MaxAnisotropy = std::get<UINT>(kDefineMap.at(sampler["MaxAnisotropy"].get<std::string>()));
            } else if (sampler["MaxAnisotropy"].is_number()) {
                samplerDesc.MaxAnisotropy = sampler["MaxAnisotropy"].get<UINT>();
            }
        }
        if (sampler.contains("ComparisonFunc")) {
            samplerDesc.ComparisonFunc = kComparisonFuncMap.at(sampler["ComparisonFunc"].get<std::string>());
        }
        if (sampler.contains("BorderColor")) {
            samplerDesc.BorderColor = kStaticBorderColorMap.at(sampler["BorderColor"].get<std::string>());
        }
        if (sampler.contains("MinLOD")) {
            if (sampler["MinLOD"].is_string()) {
                samplerDesc.MinLOD = std::get<float>(kDefineMap.at(sampler["MinLOD"].get<std::string>()));
            } else if (sampler["MinLOD"].is_number()) {
                samplerDesc.MinLOD = sampler["MinLOD"].get<float>();
            }
        }
        if (sampler.contains("MaxLOD")) {
            if (sampler["MaxLOD"].is_string()) {
                samplerDesc.MaxLOD = std::get<float>(kDefineMap.at(sampler["MaxLOD"].get<std::string>()));
            } else if (sampler["MaxLOD"].is_number()) {
                samplerDesc.MaxLOD = sampler["MaxLOD"].get<float>();
            }
        }
        if (sampler.contains("ShaderRegister")) {
            samplerDesc.ShaderRegister = sampler["ShaderRegister"].get<UINT>();
        }
        if (sampler.contains("RegisterSpace")) {
            samplerDesc.RegisterSpace = sampler["RegisterSpace"].get<UINT>();
        }
        if (sampler.contains("ShaderVisibility")) {
            samplerDesc.ShaderVisibility = kShaderVisibilityMap.at(sampler["ShaderVisibility"].get<std::string>());
        }
        samplers.push_back(samplerDesc);
    }

    pipeLines_.sampler->AddSampler(name, samplers);
    LogSimple("Sampler loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadInputLayout(const Json &json) {
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Input layout name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading input layout: " + name, kLogLevelFlagInfo);
    for (const auto &element : json["Elements"]) {
        D3D12_INPUT_ELEMENT_DESC inputElement = {};
        if (element.contains("SemanticName")) {
            inputElement.SemanticName = _strdup(element["SemanticName"].get<std::string>().c_str());
        }
        if (element.contains("SemanticIndex")) {
            inputElement.SemanticIndex = element["SemanticIndex"].get<UINT>();
        }
        if (element.contains("Format")) {
            inputElement.Format = kDxgiFormatMap.at(element["Format"].get<std::string>());
        }
        if (element.contains("InputSlot")) {
            inputElement.InputSlot = element["InputSlot"].get<UINT>();
        }
        if (element.contains("AlignedByteOffset")) {
            if (element["AlignedByteOffset"].is_string()) {
                // 文字列の場合はマップから取得
                inputElement.AlignedByteOffset = std::get<UINT>(kDefineMap.at(element["AlignedByteOffset"].get<std::string>()));
            } else if (element["AlignedByteOffset"].is_number()) {
                // 数値の場合はそのまま使用
                inputElement.AlignedByteOffset = element["AlignedByteOffset"].get<UINT>();
            }
        } else {
            // AlignedByteOffsetが指定されていない場合は D3D12_APPEND_ALIGNED_ELEMENT を使用
            inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        }
        if (element.contains("InputSlotClass")) {
            inputElement.InputSlotClass = kInputClassificationMap.at(element["InputSlotClass"].get<std::string>());
        }
        if (element.contains("InstanceDataStepRate")) {
            inputElement.InstanceDataStepRate = element["InstanceDataStepRate"].get<UINT>();
        }
        inputLayout.push_back(inputElement);
    }
    
    pipeLines_.inputLayout->AddInputLayout(name, inputLayout);
    LogSimple("Input layout loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadRasterizerState(const Json &json) {
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Rasterizer state name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading rasterizer state: " + name, kLogLevelFlagInfo);
    if (json.contains("FillMode")) {
        rasterizerDesc.FillMode = kFillModeMap.at(json["FillMode"].get<std::string>());
    }
    if (json.contains("CullMode")) {
        rasterizerDesc.CullMode = kCullModeMap.at(json["CullMode"].get<std::string>());
    }
    if (json.contains("FrontCounterClockwise")) {
        rasterizerDesc.FrontCounterClockwise = json["FrontCounterClockwise"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("DepthBias")) {
        rasterizerDesc.DepthBias = json["DepthBias"].get<INT>();
    }
    if (json.contains("DepthBiasClamp")) {
        rasterizerDesc.DepthBiasClamp = json["DepthBiasClamp"].get<float>();
    }
    if (json.contains("SlopeScaledDepthBias")) {
        rasterizerDesc.SlopeScaledDepthBias = json["SlopeScaledDepthBias"].get<float>();
    }
    if (json.contains("DepthClipEnable")) {
        rasterizerDesc.DepthClipEnable = json["DepthClipEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("MultisampleEnable")) {
        rasterizerDesc.MultisampleEnable = json["MultisampleEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("AntialiasedLineEnable")) {
        rasterizerDesc.AntialiasedLineEnable = json["AntialiasedLineEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("ForcedSampleCount")) {
        rasterizerDesc.ForcedSampleCount = json["ForcedSampleCount"].get<UINT>();
    }
    
    pipeLines_.rasterizerState->AddRasterizerState(name, rasterizerDesc);
    LogSimple("Rasterizer state loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadBlendState(const Json &json) {
    D3D12_BLEND_DESC blendDesc = {};
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Blend state name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading blend state: " + name, kLogLevelFlagInfo);

    if (json.contains("AlphaToCoverageEnable")) {
        blendDesc.AlphaToCoverageEnable = json["AlphaToCoverageEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("IndependentBlendEnable")) {
        blendDesc.IndependentBlendEnable = json["IndependentBlendEnable"].get<bool>() ? TRUE : FALSE;
    }
    
    UINT index = 0;
    for (const auto &target : json["RenderTargets"]) {
        D3D12_RENDER_TARGET_BLEND_DESC targetDesc = {};
        if (target.contains("BlendEnable")) {
            targetDesc.BlendEnable = target["BlendEnable"].get<bool>() ? TRUE : FALSE;
        }
        if (target.contains("LogicOpEnable")) {
            targetDesc.LogicOpEnable = target["LogicOpEnable"].get<bool>() ? TRUE : FALSE;
        }
        if (target.contains("SrcBlend")) {
            targetDesc.SrcBlend = kBlendMap.at(target["SrcBlend"].get<std::string>());
        }
        if (target.contains("DestBlend")) {
            targetDesc.DestBlend = kBlendMap.at(target["DestBlend"].get<std::string>());
        }
        if (target.contains("BlendOp")) {
            targetDesc.BlendOp = kBlendOpMap.at(target["BlendOp"].get<std::string>());
        }
        if (target.contains("SrcBlendAlpha")) {
            targetDesc.SrcBlendAlpha = kBlendMap.at(target["SrcBlendAlpha"].get<std::string>());
        }
        if (target.contains("DestBlendAlpha")) {
            targetDesc.DestBlendAlpha = kBlendMap.at(target["DestBlendAlpha"].get<std::string>());
        }
        if (target.contains("BlendOpAlpha")) {
            targetDesc.BlendOpAlpha = kBlendOpMap.at(target["BlendOpAlpha"].get<std::string>());
        }
        if (target.contains("LogicOp")) {
            targetDesc.LogicOp = kLogicOpMap.at(target["LogicOp"].get<std::string>());
        }
        if (target.contains("RenderTargetWriteMask")) {
            targetDesc.RenderTargetWriteMask = kColorWriteEnableMap.at(target["RenderTargetWriteMask"].get<std::string>());
        }
        blendDesc.RenderTarget[index] = targetDesc;
        index++;
        // 要素数がD3D12_SIMULTANEOUS_RENDER_TARGET_COUNTを超える場合は警告を出す
        if (index >= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT) {
            LogSimple("Number of render targets exceeds D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT.", kLogLevelFlagWarning);
            break;
        }
    }

    pipeLines_.blendState->AddBlendState(name, blendDesc);
    LogSimple("Blend state loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadShader(const Json &json) {
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Shader name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading shader: " + name, kLogLevelFlagInfo);

    std::string shaderType;
    if (json.contains("Type")) {
        shaderType = json["Type"].get<std::string>();
        if (shaderType != "Vertex" && shaderType != "Pixel" && shaderType != "Geometry" &&
            shaderType != "Hull" && shaderType != "Domain" && shaderType != "Compute" &&
            shaderType != "Mesh") {
            LogSimple("Invalid shader type: " + shaderType, kLogLevelFlagWarning);
            return;
        }
    } else {
        LogSimple("Shader type is missing.", kLogLevelFlagWarning);
        return;
    }

    std::string shaderPath;
    if (json.contains("Path")) {
        shaderPath = json["Path"].get<std::string>();
    } else {
        LogSimple("Shader path is missing.", kLogLevelFlagWarning);
        return;
    }
    
    std::string targetProfile;
    if (json.contains("TargetProfile")) {
        targetProfile = json["TargetProfile"].get<std::string>();
    } else {
        LogSimple("Shader target profile is missing.", kLogLevelFlagWarning);
        return;
    }
    
    pipeLines_.shader->AddShader(name, shaderPath, targetProfile);

    bool isReflection = false;
    if (json.contains("isReflection")) {
        isReflection = json["isReflection"].get<bool>();
        if (isReflection) {
            ShaderReflectionRun(name);
            LogSimple("Shader reflection completed for: " + name, kLogLevelFlagInfo);
        } else {
            LogSimple("Shader reflection skipped for: " + name, kLogLevelFlagInfo);
        }
    }
    if (!isReflection) {
        // シェーダーリフレクションを使わない場合はjsonデータに定義されたルートパラメーターを追加
        if (json.contains("RootParameter")) {
            Json rootParametersJson = json["RootParameter"];
            // ルートパラメーターの名前をシェーダー名に設定
            rootParametersJson["Name"] = name;
            LoadRootParameter(rootParametersJson);
        } else {
            // ルートパラメーターが指定されていない場合は空のルートパラメーターを追加
            Json emptyRootParameter = {
                {"Name", name},
                {"Parameters", Json::array()}
            };
            LoadRootParameter(emptyRootParameter);
        }
    }
    // 頂点シェーダーの場合はインプットレイアウトの設定も行う
    if (shaderType == "Vertex") {
        if (json.contains("InputLayout")) {
            Json inputLayoutJson = json["InputLayout"];
            // インプットレイアウトの名前をシェーダー名に設定
            inputLayoutJson["Name"] = name;
            LoadInputLayout(inputLayoutJson);
        } else {
            // インプットレイアウトが指定されていない場合は空のインプットレイアウトを追加
            Json emptyInputLayout = {
                {"Name", name},
                {"Elements", Json::array()}
            };
            LoadInputLayout(emptyInputLayout);
        }
    }

    LogSimple("Shader loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadDepthStencilState(const Json &json) {
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Depth stencil state name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading depth stencil state: " + name, kLogLevelFlagInfo);

    if (json.contains("DepthEnable")) {
        depthStencilDesc.DepthEnable = json["DepthEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("DepthWriteMask")) {
        depthStencilDesc.DepthWriteMask = kDepthWriteMaskMap.at(json["DepthWriteMask"].get<std::string>());
    }
    if (json.contains("DepthFunc")) {
        depthStencilDesc.DepthFunc = kComparisonFuncMap.at(json["DepthFunc"].get<std::string>());
    }
    if (json.contains("StencilEnable")) {
        depthStencilDesc.StencilEnable = json["StencilEnable"].get<bool>() ? TRUE : FALSE;
    }
    if (json.contains("StencilReadMask")) {
        if (json["StencilReadMask"].is_string()) {
            // 文字列の場合はマップから取得
            int stencilReadMask = std::get<int>(kDefineMap.at(json["StencilReadMask"].get<std::string>()));
            depthStencilDesc.StencilReadMask = static_cast<UINT8>(stencilReadMask);
        } else if (json["StencilReadMask"].is_number()) {
            // 数値の場合はそのまま使用
            depthStencilDesc.StencilReadMask = json["StencilReadMask"].get<UINT8>();
        }
    }
    if (json.contains("StencilWriteMask")) {
        if (json["StencilWriteMask"].is_string()) {
            // 文字列の場合はマップから取得
            int stencilWriteMask = std::get<int>(kDefineMap.at(json["StencilWriteMask"].get<std::string>()));
            depthStencilDesc.StencilWriteMask = static_cast<UINT8>(stencilWriteMask);
        } else if (json["StencilWriteMask"].is_number()) {
            // 数値の場合はそのまま使用
            depthStencilDesc.StencilWriteMask = json["StencilWriteMask"].get<UINT8>();
        }
    }
    
    // フロントスタンシル
    if (json.contains("FrontFace")) {
        const auto &frontFace = json["FrontFace"];
        if (frontFace.contains("StencilFailOp")) {
            depthStencilDesc.FrontFace.StencilFailOp = kStencilOpMap.at(frontFace["StencilFailOp"].get<std::string>());
        }
        if (frontFace.contains("StencilDepthFailOp")) {
            depthStencilDesc.FrontFace.StencilDepthFailOp = kStencilOpMap.at(frontFace["StencilDepthFailOp"].get<std::string>());
        }
        if (frontFace.contains("StencilPassOp")) {
            depthStencilDesc.FrontFace.StencilPassOp = kStencilOpMap.at(frontFace["StencilPassOp"].get<std::string>());
        }
        if (frontFace.contains("StencilFunc")) {
            depthStencilDesc.FrontFace.StencilFunc = kComparisonFuncMap.at(frontFace["StencilFunc"].get<std::string>());
        }
    }

    // バックスタンシル
    if (json.contains("BackFace")) {
        const auto &backFace = json["BackFace"];
        if (backFace.contains("StencilFailOp")) {
            depthStencilDesc.BackFace.StencilFailOp = kStencilOpMap.at(backFace["StencilFailOp"].get<std::string>());
        }
        if (backFace.contains("StencilDepthFailOp")) {
            depthStencilDesc.BackFace.StencilDepthFailOp = kStencilOpMap.at(backFace["StencilDepthFailOp"].get<std::string>());
        }
        if (backFace.contains("StencilPassOp")) {
            depthStencilDesc.BackFace.StencilPassOp = kStencilOpMap.at(backFace["StencilPassOp"].get<std::string>());
        }
        if (backFace.contains("StencilFunc")) {
            depthStencilDesc.BackFace.StencilFunc = kComparisonFuncMap.at(backFace["StencilFunc"].get<std::string>());
        }
    }

    pipeLines_.depthStencilState->AddDepthStencilState(name, depthStencilDesc);
    LogSimple("Depth stencil state loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadGraphicsPipelineState(const Json &json) {
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Graphics pipeline state name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading graphics pipeline state: " + name, kLogLevelFlagInfo);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineDesc = {};
    if (json.contains("SampleMask")) {
        if (json["SampleMask"].is_string()) {
            // 文字列の場合はマップから取得
            pipeLineDesc.SampleMask = std::get<UINT>(kDefineMap.at(json["SampleMask"].get<std::string>()));
        } else if (json["SampleMask"].is_number()) {
            // 数値の場合はそのまま使用
            pipeLineDesc.SampleMask = json["SampleMask"].get<UINT>();
        }
    } else {
        // SampleMaskが指定されていない場合はデフォルト値を使用
        pipeLineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    }
    if (json.contains("IBStripCutValue")) {
        pipeLineDesc.IBStripCutValue = kIndexBufferStripCutValueMap.at(json["IBStripCutValue"].get<std::string>());
    }
    if (json.contains("PrimitiveTopologyType")) {
        pipeLineDesc.PrimitiveTopologyType = kPrimitiveTopologyTypeMap.at(json["PrimitiveTopologyType"].get<std::string>());
    } else {
        LogSimple("Primitive topology type is missing in graphics pipeline state JSON.", kLogLevelFlagWarning);
        return;
    }
    if (json.contains("NumRenderTargets")) {
        pipeLineDesc.NumRenderTargets = json["NumRenderTargets"].get<UINT>();
    } else {
        // NumRenderTargetsが指定されていない場合はデフォルト値を使用
        pipeLineDesc.NumRenderTargets = 1;
    }
    if (json.contains("RTVFormats")) {
        const auto &rtvFormats = json["RTVFormats"];
        for (UINT i = 0; i < pipeLineDesc.NumRenderTargets; ++i) {
            if (i < rtvFormats.size()) {
                pipeLineDesc.RTVFormats[i] = kDxgiFormatMap.at(rtvFormats[i].get<std::string>());
            } else {
                LogSimple("Not enough RTV formats provided in graphics pipeline state JSON.", kLogLevelFlagWarning);
                break;
            }
        }
    } else {
        LogSimple("RTV formats are missing in graphics pipeline state JSON.", kLogLevelFlagWarning);
        return;
    }
    if (json.contains("DSVFormat")) {
        pipeLineDesc.DSVFormat = kDxgiFormatMap.at(json["DSVFormat"].get<std::string>());
    } else {
        LogSimple("DSV format is missing in graphics pipeline state JSON.", kLogLevelFlagWarning);
        return;
    }
    if (json.contains("SampleDesc")) {
        Json sampleDescJson = json["SampleDesc"];
        if (sampleDescJson.contains("Count")) {
            pipeLineDesc.SampleDesc.Count = sampleDescJson["Count"].get<UINT>();
        } else {
            LogSimple("Sample count is missing in graphics pipeline state JSON.", kLogLevelFlagWarning);
            return;
        }
        if (sampleDescJson.contains("Quality")) {
            pipeLineDesc.SampleDesc.Quality = sampleDescJson["Quality"].get<UINT>();
        } else {
            LogSimple("Sample quality is missing in graphics pipeline state JSON.", kLogLevelFlagWarning);
            return;
        }
    } else {
        // SampleDescが指定されていない場合はデフォルト値を使用
        pipeLineDesc.SampleDesc.Count = 1;
        pipeLineDesc.SampleDesc.Quality = 0;
    }
    if (json.contains("NodeMask")) {
        pipeLineDesc.NodeMask = json["NodeMask"].get<UINT>();
    }
    if (json.contains("Flags")) {
        pipeLineDesc.Flags = kPipelineStateFlagsMap.at(json["Flags"].get<std::string>());
    } else {
        // Flagsが指定されていない場合はデフォルト値を使用
        pipeLineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }

    pipeLines_.graphicsPipeLineState->AddPipeLineState(name, pipeLineDesc);
    LogSimple("Graphics pipeline state loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::LoadComputePipelineState(const Json &json) {
    std::string name;
    if (json.contains("Name")) {
        name = json["Name"].get<std::string>();
    } else {
        LogSimple("Compute pipeline state name is missing.", kLogLevelFlagWarning);
        return;
    }
    
    LogSimple("Loading compute pipeline state: " + name, kLogLevelFlagInfo);
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipeLineDesc = {};
    if (json.contains("NodeMask")) {
        computePipeLineDesc.NodeMask = json["NodeMask"].get<UINT>();
    }
    if (json.contains("Flags")) {
        computePipeLineDesc.Flags = kPipelineStateFlagsMap.at(json["Flags"].get<std::string>());
    } else {
        // Flagsが指定されていない場合はデフォルト値を使用
        computePipeLineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    }
    pipeLines_.computePipeLineState->AddPipeLineState(name, computePipeLineDesc);
    LogSimple("Compute pipeline state loaded: " + name, kLogLevelFlagInfo);
}

void PipeLineManager::ShaderReflectionRun(const std::string &shaderName) {
    LogSimple("Running shader reflection for: " + shaderName, kLogLevelFlagInfo);
    IDxcBlob *shaderBlob = pipeLines_.shader->GetShader(shaderName);
    if (!shaderBlob) {
        LogSimple("Shader blob not found for: " + shaderName, kLogLevelFlagWarning);
        return;
    }
    
    auto shaderRefrectionInfo = shaderReflection_->GetShaderReflection(shaderBlob);
    
    // シェーダーリフレクション情報からルートパラメーターを追加
    auto rootParameters = shaderRefrectionInfo.rootParameters;
    pipeLines_.rootParameter->AddRootParameter(shaderName, rootParameters);
}


} // namespace KashipanEngine