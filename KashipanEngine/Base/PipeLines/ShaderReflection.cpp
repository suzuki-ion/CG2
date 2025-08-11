#include <cassert>
#include "Common/Logs.h"
#include "ShaderReflection.h"

namespace KashipanEngine {

namespace {
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
)
} // namespace

ShaderReflection::ShaderReflection() {
    DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&dxcLibrary_));
}

Microsoft::WRL::ComPtr<ID3D12ShaderReflection> ShaderReflection::GetShaderReflection(IDxcBlob *shaderBlob) const {
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

    Log("Shader reflection created successfully");
    return shaderReflection;
}

} // namespace KashipanEngine