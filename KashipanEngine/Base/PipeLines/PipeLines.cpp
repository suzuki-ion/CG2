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
    rootSignature_(std::make_unique<RootSignature>(sDxCommon->GetDevice())),
    rootParameter_(std::make_unique<RootParameter>()),
    descriptorRange_(std::make_unique<DescriptorRange>()),
    sampler_(std::make_unique<Sampler>()),
    inputLayout_(std::make_unique<InputLayout>()),
    rasterizerState_(std::make_unique<RasterizerState>()),
    blendState_(std::make_unique<BlendState>()),
    shaderCompiler_(std::make_unique<ShaderCompiler>()),
    depthStencilState_(std::make_unique<DepthStencilState>()) {
}

} // namespace KashipanEngine