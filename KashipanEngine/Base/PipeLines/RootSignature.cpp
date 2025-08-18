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
    if (FAILED(hr)) {
        Log("Failed to create root signature.", kLogLevelFlagError);
        assert(false);
    }
}

ID3D12RootSignature *RootSignature::CreateRootSignatureBinary(const std::string &rootSignatureName, const std::vector<D3D12_ROOT_PARAMETER> &rootParameters, const std::vector<D3D12_STATIC_SAMPLER_DESC> &staticSamplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {
    HRESULT hr;
    if (rootParameters.empty()) {
        Log("Root parameters are empty.", kLogLevelFlagError);
        assert(false);
    }

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc = rootSignatures_[rootSignatureName].rootSignatureDesc;
    rootSignatureDesc.Flags = flags;
    rootSignatureDesc.pParameters = rootParameters.data();
    rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
    if (!staticSamplers.empty()) {
        rootSignatureDesc.pStaticSamplers = staticSamplers.data();
        rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(staticSamplers.size());
    }

    hr = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob,
        &errorBlob
    );
    if (FAILED(hr)) {
        Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()), kLogLevelFlagError);
        assert(false);
    }

    // バイナリを元に生成
    ID3D12RootSignature *rootSignature = nullptr;
    hr = device_->CreateRootSignature(
        0,
        signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)
    );
    if (FAILED(hr)) {
        Log("Failed to create root signature.", kLogLevelFlagError);
        assert(false);
    }
    return rootSignature;
}

} // namespace KashipanEngine