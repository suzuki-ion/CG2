#include "PrimitiveDrawer.h"
#include "Common/VertexData.h"
#include "Common/Logs.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"

#include <cassert>
#include <format>
#include <dxcapi.h>

#pragma comment(lib, "dxcompiler.lib")

namespace KashipanEngine {
bool PrimitiveDrawer::isInitialized_ = false;
DirectXCommon *PrimitiveDrawer::dxCommon_ = nullptr;

namespace {

/// @brief シェーダーコンパイル用関数
/// @param filePath コンパイルするシェーダーファイルへのパス
/// @param profile コンパイルに使用するプロファイル
/// @param dxcUtils 初期化で生成したIDxcUtilsインターフェース
/// @param dxcCompiler 初期化で生成したIDxcCompiler3インターフェース
/// @param includeHandler インクルードファイルを扱うためのインターフェース
/// @return コンパイル結果
IDxcBlob *CompileShader(const std::wstring &filePath, const wchar_t *profile,
    IDxcUtils *dxcUtils, IDxcCompiler3 *dxcCompiler, IDxcIncludeHandler *includeHandler) {
    // これからシェーダーをコンパイルする旨をログに出力
    LogSimple(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile));

    //==================================================
    // 1. hlslファイルを読む
    //==================================================

    IDxcBlobEncoding *shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,        // 読み込んだファイル
        arguments,                  // コンパイルオプション
        _countof(arguments),        // コンパイルオプションの数
        includeHandler,             // includeが含まれた諸々
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

} // namespace

void PrimitiveDrawer::Initialize(DirectXCommon *dxCommon, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (isInitialized_) {
        Log("PrimitiveDrawer is already initialized.", kLogLevelFlagError);
        assert(false);
    }

    // nullチェック
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }

    // 引数をメンバ変数に格納
    dxCommon_ = dxCommon;
    // 初期化済みフラグを立てる
    isInitialized_ = true;

    // 初期化完了のログを出力
    LogSimple("PrimitiveDrawer Initialized.");
    LogNewLine();
}

Microsoft::WRL::ComPtr<ID3D12Resource> PrimitiveDrawer::CreateBufferResources(UINT64 size, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("PrimitiveDrawer is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // ヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
    // リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size; // リソースのサイズ
    // バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // リソースの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    // リソースの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // ログに生成したリソースのサイズを出力
    LogSimple(std::format("CreateBufferResources, size:{}", size));
    return resource;
}

PipeLineSet PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode, const bool isDepthEnable, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);

    //==================================================
    // ルートシグネチャの生成
    //==================================================

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    //--------- RootParameter作成 ---------//

    D3D12_ROOT_PARAMETER rootParameters[4]{};
    // PixelShaderのMaterial
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderを使う
    rootParameters[0].Descriptor.ShaderRegister = 0;                    // レジスタ番号0を使う
    // VertexShaderのTransform
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
    rootParameters[1].Descriptor.ShaderRegister = 0;                    // レジスタ番号0を使う

    // DescriptorRange
    D3D12_DESCRIPTOR_RANGE descriptorRange[1]{};
    descriptorRange[0].BaseShaderRegister = 0;                                                      // 0から始まる
    descriptorRange[0].NumDescriptors = 1;                                                          // 数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                                 // SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;    // Offsetを自動計算

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;       // DescriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                 // PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;              // Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);  // Tableで利用する数

    // 平行光源
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderを使う
    rootParameters[3].Descriptor.ShaderRegister = 1;                    // レジスタ番号1を使う

    descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

    // Samplerの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;         // バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;       // 0～1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;     // 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;                       // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0;                               // レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    // シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()), kLogLevelFlagError);
        assert(false);
    }

    // バイナリを元に生成
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    //==================================================
    // InputLayoutの設定
    //==================================================

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    //==================================================
    // RasterizerStateの設定
    //==================================================

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    if (topologyType == D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE) {
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    } else {
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }

    //==================================================
    // BlendStateの設定
    //==================================================

    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

    switch (blendMode) {
        case kBlendModeNone:
            blendDesc.RenderTarget[0].BlendEnable = false;
            break;
        case kBlendModeNormal:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            break;
        case kBlendModeAdd:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            break;
        case kBlendModeSubtract:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            break;
        case kBlendModeMultiply:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
            break;
        case kBlendModeScreen:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
            break;
        case kBlendModeExclusion:
            blendDesc.RenderTarget[0].BlendEnable = true;
            blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
            blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
            break;
        default:
            break;
    } 

    //==================================================
    // Shaderをコンパイルする
    //==================================================

    //dxcCompilerを初期化
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // includeに対応するための設定
    IDxcIncludeHandler *includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(
        L"KashipanEngine/Shader/Object3d.VS.hlsl", L"vs_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(
        L"KashipanEngine/Shader/Object3d.PS.hlsl", L"ps_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);

    //==================================================
    // DepthStencilStateの設定
    //==================================================

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    if (isDepthEnable) {
        // 深度バッファを使う
        depthStencilDesc.DepthEnable = true;                            // Depthの機能を有効化する
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;   // 深度値を書き込む
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;  // 比較関数
    } else {
        // 深度バッファを使わない
        depthStencilDesc.DepthEnable = false;                           // Depthの機能を無効化する
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;  // 深度値を書き込まない
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;      // 比較関数
    }

    //==================================================
    // PSOを生成する
    //==================================================

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // ルートシグネチャ
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;        // InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize() };                        // VertexShader
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize() };                         // PixelShader
    graphicsPipelineStateDesc.BlendState = blendDesc;               // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;     // RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] =
        (blendMode != kBlendModeExclusion)
        ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
        : DXGI_FORMAT_R8G8B8A8_UNORM;
    // 利用するトポロジ（形状）のタイプ
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; //topologyType;
    // どのように画面に色を打ち込むかの設定
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 深度バッファの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // 生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    //==================================================
    // 生成したものをセットにして返す
    //==================================================

    PipeLineSet pipelineSet;
    pipelineSet.rootSignature = rootSignature;
    pipelineSet.pipelineState = pipelineState;

    // ログに生成完了のメッセージを出力
    LogSimple("CreateGraphicsPipeline Succeeded.");
    return pipelineSet;
}

PipeLineSet PrimitiveDrawer::CreateLinePipeline(LineType lineType, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);

    //==================================================
    // ルートシグネチャの生成
    //==================================================

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    //--------- RootParameter作成 ---------//

    D3D12_ROOT_PARAMETER rootParameters[2]{};
    // VertexShaderのTransform
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
    rootParameters[0].Descriptor.ShaderRegister = 0;                    // レジスタ番号0を使う
    // VertexShaderのLineOption
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
    rootParameters[1].Descriptor.ShaderRegister = 1;                    // レジスタ番号1を使う

    // DescriptorRange
    D3D12_DESCRIPTOR_RANGE descriptorRange[1]{};
    descriptorRange[0].BaseShaderRegister = 0;                                                      // 0から始まる
    descriptorRange[0].NumDescriptors = 1;                                                          // 数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                                 // SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;    // Offsetを自動計算

    descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

    // シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()), kLogLevelFlagError);
        assert(false);
    }

    // バイナリを元に生成
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    //==================================================
    // InputLayoutの設定
    //==================================================

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "COLOR";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "WIDTH";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[3].SemanticName = "HEIGHT";
    inputElementDescs[3].SemanticIndex = 0;
    inputElementDescs[3].Format = DXGI_FORMAT_R32_FLOAT;
    inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[4].SemanticName = "DEPTH";
    inputElementDescs[4].SemanticIndex = 0;
    inputElementDescs[4].Format = DXGI_FORMAT_R32_FLOAT;
    inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    //==================================================
    // RasterizerStateの設定
    //==================================================

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    //==================================================
    // BlendStateの設定
    //==================================================

    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    //==================================================
    // Shaderをコンパイルする
    //==================================================

    //dxcCompilerを初期化
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // includeに対応するための設定
    IDxcIncludeHandler *includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(
        L"KashipanEngine/Shader/Line.VS.hlsl", L"vs_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);
    Microsoft::WRL::ComPtr<IDxcBlob> GeometryShaderBlob;
    if (lineType == kLineThickness) {
        GeometryShaderBlob = CompileShader(
            L"KashipanEngine/Shader/Line.GS.hlsl", L"gs_6_0",
            dxcUtils.Get(), dxcCompiler.Get(), includeHandler);
    }
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(
        L"KashipanEngine/Shader/Line.PS.hlsl", L"ps_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);

    //==================================================
    // DepthStencilStateの設定
    //==================================================

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    // 深度バッファを使う
    depthStencilDesc.DepthEnable = true;                            // Depthの機能を有効化する
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;   // 深度値を書き込む
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;  // 比較関数

    //==================================================
    // PSOを生成する
    //==================================================

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // ルートシグネチャ
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;        // InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize() };                        // VertexShader
    if (lineType == kLineThickness) {
        graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
            GeometryShaderBlob->GetBufferSize() };                  // GeometryShader
    }
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize() };                         // PixelShader
    graphicsPipelineStateDesc.BlendState = blendDesc;               // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;     // RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ（形状）のタイプ
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    // どのように画面に色を打ち込むかの設定
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 深度バッファの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // 生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    //==================================================
    // 生成したものをセットにして返す
    //==================================================

    PipeLineSet pipelineSet;
    pipelineSet.rootSignature = rootSignature;
    pipelineSet.pipelineState = pipelineState;

    // ログに生成完了のメッセージを出力
    LogSimple("CreateGraphicsPipeline Succeeded.");
    return pipelineSet;
}

PrimitiveDrawer::IntermediateMesh PrimitiveDrawer::CreateIntermediateMesh(UINT vertexCount, UINT indexCount, unsigned long long vertexStride, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);

    // 頂点バッファの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer =
        CreateBufferResources(static_cast<UINT>(vertexStride) * vertexCount);
    // インデックスバッファの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer =
        CreateBufferResources(sizeof(uint32_t) * indexCount);

    //==================================================
    // 頂点バッファの設定
    //==================================================

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // リソースの先頭アドレスから使う
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    // 使用するリソースのサイズ
    vertexBufferView.SizeInBytes = static_cast<UINT>(vertexStride) * vertexCount;
    // 1頂点あたりのサイズ
    vertexBufferView.StrideInBytes = static_cast<UINT>(vertexStride);

    //==================================================
    // インデックスバッファの設定
    //==================================================

    // インデックスバッファビューを作成する
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    // リソースの先頭アドレスから使う
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    // 使用するリソースのサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * indexCount;
    // フォーマット
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    // 中間メッシュを返す
    IntermediateMesh intermediateMesh;
    intermediateMesh.vertexBuffer = vertexBuffer;
    intermediateMesh.indexBuffer = indexBuffer;
    intermediateMesh.vertexBufferView = vertexBufferView;
    intermediateMesh.indexBufferView = indexBufferView;

    // ログに生成したメッシュの頂点数とインデックス数を出力
    LogSimple(std::format("CreateMesh, vertexCount:{}, indexCount:{}", vertexCount, indexCount));

    return intermediateMesh;
}

} // namespace KashipanEngine