#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>

#include "RootSignature.h"
#include "RootParameter.h"
#include "DescriptorRange.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "Shader.h"
#include "DepthStencilState.h"

namespace KashipanEngine {

class DirectXCommon;

struct PipeLines {
    static void Initialize(DirectXCommon *dxCommon);
    PipeLines();
    ~PipeLines() = default;
    const std::unique_ptr<RootSignature> rootSignature_;
    const std::unique_ptr<RootParameter> rootParameter_;
    const std::unique_ptr<DescriptorRange> descriptorRange_;
    const std::unique_ptr<Sampler> sampler_;
    const std::unique_ptr<InputLayout> inputLayout_;
    const std::unique_ptr<RasterizerState> rasterizerState_;
    const std::unique_ptr<BlendState> blendState_;
    const std::unique_ptr<Shader> shader_;
    const std::unique_ptr<DepthStencilState> depthStencilState_;
};

} // namespace KashipanEngine