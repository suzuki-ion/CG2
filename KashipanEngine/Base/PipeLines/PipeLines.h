#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>

#include "RootSignature.h"
#include "RootParameter.h"
#include "DescriptorRange.h"
#include "RootConstants.h"
#include "RootDescriptor.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "Shader.h"
#include "DepthStencilState.h"
#include "GraphicsPipeLineState.h"
#include "ComputePipeLineState.h"

namespace KashipanEngine {

class DirectXCommon;

struct PipeLines {
    static void Initialize(DirectXCommon *dxCommon);
    PipeLines();
    ~PipeLines() = default;
    void Reset();
    const std::unique_ptr<RootSignature> rootSignature;
    const std::unique_ptr<RootParameter> rootParameter;
    const std::unique_ptr<DescriptorRange> descriptorRange;
    const std::unique_ptr<RootConstants> rootConstants;
    const std::unique_ptr<RootDescriptor> rootDescriptor;
    const std::unique_ptr<Sampler> sampler;
    const std::unique_ptr<InputLayout> inputLayout;
    const std::unique_ptr<RasterizerState> rasterizerState;
    const std::unique_ptr<BlendState> blendState;
    const std::unique_ptr<Shader> shader;
    const std::unique_ptr<DepthStencilState> depthStencilState;
    const std::unique_ptr<GraphicsPipeLineState> graphicsPipeLineState;
    const std::unique_ptr<ComputePipeLineState> computePipeLineState;
};

} // namespace KashipanEngine