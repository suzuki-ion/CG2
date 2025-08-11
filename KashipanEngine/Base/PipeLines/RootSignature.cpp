#include <cassert>
#include "Common/Logs.h"
#include "RootSignature.h"

namespace KashipanEngine {

void RootSignature::SetRootSignature(const std::string &rootSignatureName, const std::vector<D3D12_ROOT_PARAMETER> &rootParameters, const std::vector<D3D12_STATIC_SAMPLER_DESC> &staticSamplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {
    HRESULT hr;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    hr = D3D12SerializeRootSignature(
        &rootSignatures_[rootSignatureName].rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob,
        &errorBlob
    );
    if (FAILED(hr)) {
        Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()), kLogLevelFlagError);
        assert(false);
    }

    // バイナリを元に生成
    hr = device_->CreateRootSignature(
        0,
        signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignatures_[rootSignatureName].rootSignature)
    );
}

} // namespace KashipanEngine