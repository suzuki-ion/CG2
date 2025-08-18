#include <cassert>
#include <vector>
#include "Common/Logs.h"
#include "ShaderReflection.h"

namespace KashipanEngine {

namespace {
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
)
} // namespace

ShaderReflection::ShaderReflection(ID3D12Device *device) {
    assert(device != nullptr);
    DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&dxcLibrary_));
    if (!dxcLibrary_) {
        LogSimple("Failed to create DxcLibrary instance", kLogLevelFlagError);
        assert(false);
    }
    device_ = device;
}

ShaderReflectionInfo ShaderReflection::GetShaderReflection(IDxcBlob *shaderBlob) const {
    ShaderReflectionInfo shaderReflectionInfo;
    HRESULT hr;

    Log("Creating shader reflection");
    if (!shaderBlob) {
        LogSimple("ShaderBlob is null", kLogLevelFlagError);
        assert(false);
    }

    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderReflection;
    Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderBlobEncoding;
    Microsoft::WRL::ComPtr<IDxcContainerReflection> containerReflection;

    hr = dxcLibrary_->CreateBlobWithEncodingOnHeapCopy(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        CP_ACP, &shaderBlobEncoding
    );
    if (FAILED(hr)) {
        LogSimple("Failed to create blob with encoding", kLogLevelFlagError);
        assert(false);
    }

    hr = DxcCreateInstance(
        CLSID_DxcContainerReflection,
        IID_PPV_ARGS(&containerReflection)
    );
    if (FAILED(hr)) {
        LogSimple("Failed to create container reflection", kLogLevelFlagError);
        assert(false);
    }

    hr = containerReflection->Load(shaderBlobEncoding.Get());
    if (FAILED(hr)) {
        LogSimple("Failed to load shader blob into container reflection", kLogLevelFlagError);
        assert(false);
    }

    UINT shaderCount = 0;
    hr = containerReflection->FindFirstPartKind(DXIL_FOURCC('D', 'X', 'I', 'L'), &shaderCount);
    if (FAILED(hr) || shaderCount == 0) {
        LogSimple("Failed to find DXIL part in shader blob", kLogLevelFlagError);
        assert(false);
    }

    hr = containerReflection->GetPartReflection(shaderCount, IID_PPV_ARGS(&shaderReflection));
    if (FAILED(hr)) {
        LogSimple("Failed to get shader reflection from container", kLogLevelFlagError);
        assert(false);
    }

    LogSimple("Shader reflection created successfully");
    LogSimple("Creating root parameters from shader reflection");
    shaderReflectionInfo.rootParameters = CreateRootParameters(shaderReflection.Get());
    LogSimple("Root parameters created successfully");
    LogSimple("Creating input layout from shader reflection");
    shaderReflectionInfo.inputLayout = CreateInputLayout(shaderReflection.Get());
    LogSimple("Input layout created successfully");

    shaderReflectionInfo.shaderReflection = shaderReflection;
    LogSimple("Shader reflection completed successfully");
    return shaderReflectionInfo;
}

const std::vector<D3D12_ROOT_PARAMETER> &ShaderReflection::CreateRootParameters(ID3D12ShaderReflection *shaderReflectionInfo) const {
    D3D12_SHADER_DESC shaderDesc;
    HRESULT hr = shaderReflectionInfo->GetDesc(&shaderDesc);
    if (FAILED(hr)) {
        LogSimple("Failed to get shader description", kLogLevelFlagError);
        assert(false);
    }

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    UINT rootParameterIndex = 0;
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        hr = shaderReflectionInfo->GetResourceBindingDesc(i, &bindDesc);
        if (FAILED(hr)) {
            LogSimple("Failed to get resource binding description", kLogLevelFlagError);
            assert(false);
        }

        switch (bindDesc.Type) {
            case D3D_SIT_CBUFFER: {
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
                rootParam.Descriptor.RegisterSpace = bindDesc.Space;
                rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.push_back(rootParam);
                break;
            }
            case D3D_SIT_TEXTURE: {
                D3D12_DESCRIPTOR_RANGE srvRange = {};
                srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                srvRange.NumDescriptors = 1;
                srvRange.BaseShaderRegister = bindDesc.BindPoint;
                srvRange.RegisterSpace = bindDesc.Space;
                srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = &srvRange;
                rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.push_back(rootParam);
                break;
            }
            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_RWBYTEADDRESS:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED: {
                D3D12_DESCRIPTOR_RANGE uavRange = {};
                uavRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                uavRange.NumDescriptors = 1;
                uavRange.BaseShaderRegister = bindDesc.BindPoint;
                uavRange.RegisterSpace = bindDesc.Space;
                uavRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = &uavRange;
                rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.push_back(rootParam);
                break;
            }
            case D3D_SIT_SAMPLER: {
                D3D12_DESCRIPTOR_RANGE samplerRange = {};
                samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                samplerRange.NumDescriptors = 1;
                samplerRange.BaseShaderRegister = bindDesc.BindPoint;
                samplerRange.RegisterSpace = bindDesc.Space;
                samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = &samplerRange;
                rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.push_back(rootParam);
                break;
            }
        }
    }

    if (rootParameters.empty()) {
        LogSimple("No root parameters found in shader reflection", kLogLevelFlagWarning);
    } else {
        Log("Root parameters created successfully");
    }
    return rootParameters;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC> &ShaderReflection::CreateInputLayout(ID3D12ShaderReflection *shaderReflectionInfo) const {
    D3D12_SHADER_DESC shaderDesc;
    HRESULT hr = shaderReflectionInfo->GetDesc(&shaderDesc);
    if (FAILED(hr)) {
        LogSimple("Failed to get shader description for input layout", kLogLevelFlagError);
        assert(false);
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
    for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
        D3D12_SIGNATURE_PARAMETER_DESC paramDesc = {};
        hr = shaderReflectionInfo->GetInputParameterDesc(i, &paramDesc);
        if (FAILED(hr)) {
            LogSimple("Failed to get input parameter description", kLogLevelFlagError);
            assert(false);
        }
        D3D12_INPUT_ELEMENT_DESC elementDesc = {};
        elementDesc.SemanticName = paramDesc.SemanticName;
        elementDesc.SemanticIndex = paramDesc.SemanticIndex;
        elementDesc.Format = DXGI_FORMAT_UNKNOWN; // Format needs to be set based on the semantic type
        elementDesc.InputSlot = 0; // Default slot
        elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT; // Append aligned
        elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;
        inputLayout.push_back(elementDesc);
    }
    if (inputLayout.empty()) {
        LogSimple("No input layout found in shader reflection", kLogLevelFlagWarning);
    } else {
        Log("Input layout created successfully");
    }
    return inputLayout;
}

} // namespace KashipanEngine