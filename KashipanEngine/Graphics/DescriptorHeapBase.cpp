#include "DescriptorHeapBase.h"

namespace KashipanEngine {

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapBase::GetCPUHandle(UINT index) {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap_->GetCPUDescriptorHandleForHeapStart(), index, descriptorSize_);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapBase::GetGPUHandle(UINT index) {
    if (!isShaderVisible_) {
        assert(false && "Heap is not shader visible.");
        return {};
    }
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(heap_->GetGPUDescriptorHandleForHeapStart(), gpuHandleIndex++, descriptorSize_);
}

CPUDescriptorHandleInfo DescriptorHeapBase::GetCPUHandleInfo() {
    if (cpuHandleIndex >= numDescriptors_) {
        assert(false && "Exceeded the number of descriptors.");
        return CPUDescriptorHandleInfo({}, 0);
    }
    return CPUDescriptorHandleInfo(GetCPUHandle(cpuHandleIndex++), cpuHandleIndex - 1);
}

GPUDescriptorHandleInfo DescriptorHeapBase::GetGPUHandleInfo() {
    if (!isShaderVisible_) {
        assert(false && "Heap is not shader visible.");
        return GPUDescriptorHandleInfo({}, 0);
    }
    if (gpuHandleIndex >= numDescriptors_) {
        assert(false && "Exceeded the number of descriptors.");
        return GPUDescriptorHandleInfo({}, 0);
    }
    return GPUDescriptorHandleInfo(GetGPUHandle(gpuHandleIndex - 1), gpuHandleIndex - 1);
}

} // namespace KashipanEngine