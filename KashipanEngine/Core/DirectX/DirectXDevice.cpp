#include "DirectXDevice.h"
#include "Common/Logs.h"
#include <Windows.h>
#include <cassert>
#include <format>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace KashipanEngine {

DirectXDevice::DirectXDevice(bool enableDebugLayer) : fenceValue_(0), fenceEvent_(nullptr) {
    SetupDebugLayer(enableDebugLayer);
    
    InitializeDXGI();
    InitializeCommandQueue();
    InitializeCommandAllocator();
    InitializeCommandList();
    InitializeFence();

    Log("DirectXDevice initialized.");
}

DirectXDevice::~DirectXDevice() {
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
    }
    Log("DirectXDevice finalized.");
}

void DirectXDevice::ResetCommandList() {
    HRESULT hr = commandAllocator_->Reset();
    assert(SUCCEEDED(hr));
    
    hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

void DirectXDevice::ExecuteCommand() {
    // コマンドリストを確定
    HRESULT hr = commandList_->Close();
    assert(SUCCEEDED(hr));

    // コマンドを実行
    ID3D12CommandList* commandLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists);

    // フェンス処理
    fenceValue_++;
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    if (fence_->GetCompletedValue() < fenceValue_) {
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }

    // 次のフレーム用にリセット
    ResetCommandList();
}

void DirectXDevice::CreateDescriptorHeap(
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    UINT numDescriptors,
    bool shaderVisible
) {
    descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    
    HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));
}

void DirectXDevice::SetupDebugLayer(bool enableDebugLayer) {
#ifdef _DEBUG
    if (enableDebugLayer) {
        ID3D12Debug1* debugController = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            debugController->SetEnableGPUBasedValidation(true);
        }
    }
#else
    static_cast<void>(enableDebugLayer);
#endif
}

void DirectXDevice::InitializeDXGI() {
    // DXGIファクトリーの生成
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    assert(SUCCEEDED(hr));

    InitializeDXGIAdapter();
    InitializeD3D12Device();

    Log("DXGI initialized.");

    // デバッグ情報キューの設定
#ifdef _DEBUG
    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        infoQueue->PushStorageFilter(&filter);

        infoQueue->Release();
    }
#endif
}

void DirectXDevice::InitializeDXGIAdapter() {
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr));

        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            Log(std::format(L"Using adapter: {}", adapterDesc.Description));
            break;
        }
        useAdapter_ = nullptr;
    }
    assert(useAdapter_ != nullptr);
}

void DirectXDevice::InitializeD3D12Device() {
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = {
        "12.2", "12.1", "12.0"
    };

    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        if (SUCCEEDED(hr)) {
            Log(std::format("D3D12 Feature Level: {}", featureLevelStrings[i]));
            break;
        }
    }
    assert(device_ != nullptr);
}

void DirectXDevice::InitializeCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    assert(SUCCEEDED(hr));
    Log("Command queue initialized.");
}

void DirectXDevice::InitializeCommandAllocator() {
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    assert(SUCCEEDED(hr));
    Log("Command allocator initialized.");
}

void DirectXDevice::InitializeCommandList() {
    HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    assert(SUCCEEDED(hr));
    Log("Command list initialized.");
}

void DirectXDevice::InitializeFence() {
    fenceValue_ = 0;
    HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));

    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_ != nullptr);
    
    Log("Fence initialized.");
}

} // namespace KashipanEngine