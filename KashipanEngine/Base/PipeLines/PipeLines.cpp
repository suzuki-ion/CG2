#include "Base/DirectXCommon.h"
#include "PipeLines.h"

namespace KashipanEngine {

namespace {
// DirectXCommonへのポインタ
DirectXCommon *sDxCommon = nullptr;
} // namespace

void PipeLines::Initialize(DirectXCommon *dxCommon) {
    sDxCommon = dxCommon;
}

PipeLines::PipeLines() :
    rootSignature(std::make_unique<RootSignature>(sDxCommon->GetDevice())),
    rootParameter(std::make_unique<RootParameter>()),
    descriptorRange(std::make_unique<DescriptorRange>()),
    rootConstants(std::make_unique<RootConstants>()),
    rootDescriptor(std::make_unique<RootDescriptor>()),
    sampler(std::make_unique<Sampler>()),
    inputLayout(std::make_unique<InputLayout>()),
    rasterizerState(std::make_unique<RasterizerState>()),
    blendState(std::make_unique<BlendState>()),
    shader(std::make_unique<Shader>()),
    depthStencilState(std::make_unique<DepthStencilState>()),
    graphicsPipeLineState(std::make_unique<GraphicsPipeLineState>()),
    computePipeLineState(std::make_unique<ComputePipeLineState>())
{}

void PipeLines::Reset() {
    rootSignature->Clear();
    rootParameter->Clear();
    descriptorRange->Clear();
    rootConstants->Clear();
    rootDescriptor->Clear();
    sampler->Clear();
    inputLayout->Clear();
    rasterizerState->Clear();
    blendState->Clear();
    shader->Clear();
    depthStencilState->Clear();
    graphicsPipeLineState->Clear();
    computePipeLineState->Clear();
}

} // namespace KashipanEngine