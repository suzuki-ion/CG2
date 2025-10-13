#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>

#include "Base/PipeLines/RootSignature.h"
#include "Base/PipeLines/RootParameter.h"
#include "Base/PipeLines/DescriptorRange.h"
#include "Base/PipeLines/RootConstants.h"
#include "Base/PipeLines/RootDescriptor.h"
#include "Base/PipeLines/Sampler.h"
#include "Base/PipeLines/InputLayout.h"
#include "Base/PipeLines/RasterizerState.h"
#include "Base/PipeLines/BlendState.h"
#include "Base/PipeLines/Shader.h"
#include "Base/PipeLines/DepthStencilState.h"
#include "Base/PipeLines/GraphicsPipeLineState.h"
#include "Base/PipeLines/ComputePipeLineState.h"

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