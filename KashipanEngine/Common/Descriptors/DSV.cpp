#include <cassert>
#include <d3dx12.h>

#include "DSV.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Common/Logs.h"

namespace KashipanEngine {

bool DSV::isInitialized_ = false;
DirectXCommon *DSV::dxCommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DSV::descriptorHeap_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> DSV::depthStencilResource_ = nullptr;
uint32_t DSV::nextIndexCPU_ = 0;
uint32_t DSV::nextIndexGPU_ = 0;

void DSV::Initialize(WinApp *winApp, DirectXCommon *dxCommon, const std::source_location &location) {
    static bool isDSVInitialized = false;

    // 呼び出された場所のログを出力
    Log(location);

    // 初回だけnullチェックと変数の初期化をする
    if (!isDSVInitialized) {
        // nullチェック
        if (dxCommon == nullptr) {
            Log("dxCommon is null.", kLogLevelFlagError);
            assert(false);
        }

        // 引数をメンバ変数に格納
        dxCommon_ = dxCommon;
        
    }

    //==================================================
    // 深度ステンシルビュー用のリソースを生成
    //==================================================

    depthStencilResource_ = dxCommon->CreateDepthStencilTextureResource(
        winApp->GetClientWidth(),
        winApp->GetClientHeight(),
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );
    isInitialized_ = true;

    //==================================================
    // DSV用のヒープを生成
    //==================================================

    // DSV用のヒープ。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dxCommon_->CreateDescriptorHeap(
        descriptorHeap_,
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        numDescriptors_,
        false
    );

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;         // Format。基本的にはResourceと合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ

    // DSVHeapの先頭にDSVを作成
    dxCommon_->GetDevice()->CreateDepthStencilView(
        depthStencilResource_.Get(),
        &dsvDesc,
        GetCPUDescriptorHandle(0)
    );

    // 初期化完了のログを出力
    if (!isDSVInitialized) {
        LogSimple("Complete Initialize DSV.", kLogLevelFlagInfo);
        // 初期化済みフラグを立てる
        isDSVInitialized = true;
    } else {
        LogSimple("Reinitialize DSV.", kLogLevelFlagInfo);
    }
}

void DSV::Finalize(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // DescriptorHeapの解放
    descriptorHeap_.Reset();
    // DepthStencilResourceの解放
    depthStencilResource_.Reset();

    // 終了処理完了のログを出力
    Log("DSV Finalized.");
}

ID3D12DescriptorHeap *DSV::GetDescriptorHeap(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // DSVHeapのポインタを返す
    return descriptorHeap_.Get();
}

ID3D12Resource *DSV::GetDepthStencilResource(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // DepthStencilResourceのポインタを返す
    return depthStencilResource_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DSV::GetCPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // インデックスのオーバーフローをチェック
    if (nextIndexCPU_ >= numDescriptors_) {
        Log("CPU DescriptorHandle index overflow.", kLogLevelFlagError);
        assert(false);
    }
    // 返すハンドルのインデックス数をログに出力
    LogSimple("CPU DescriptorHandle index: " + std::to_string(nextIndexCPU_));

    // ディスクリプタヒープのCPUハンドルを返す
    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * nextIndexCPU_;
    nextIndexCPU_++;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DSV::GetGPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // インデックスのオーバーフローをチェック
    if (nextIndexGPU_ >= numDescriptors_) {
        Log("GPU DescriptorHandle index overflow.", kLogLevelFlagError);
        assert(false);
    }
    // 返すハンドルのインデックス数をログに出力
    LogSimple("GPU DescriptorHandle index: " + std::to_string(nextIndexGPU_));

    // ディスクリプタヒープのGPUハンドルを返す
    D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * nextIndexGPU_;
    nextIndexGPU_++;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DSV::GetCPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // インデックスのオーバーフローをチェック
    if (index >= numDescriptors_) {
        Log("CPU DescriptorHandle index overflow.", kLogLevelFlagError);
        assert(false);
    }
    // 返すハンドルのインデックス数をログに出力
    LogSimple("CPU DescriptorHandle index: " + std::to_string(index));

    // ディスクリプタヒープのCPUハンドルを返す
    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * index;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DSV::GetGPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // インデックスのオーバーフローをチェック
    if (index >= numDescriptors_) {
        Log("GPU DescriptorHandle index overflow.", kLogLevelFlagError);
        assert(false);
    }
    // 返すハンドルのインデックス数をログに出力
    LogSimple("GPU DescriptorHandle index: " + std::to_string(index));
    // ディスクリプタヒープのGPUハンドルを返す
    D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * index;
    return handle;
}

uint32_t DSV::GetNextIndexCPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexCPU_++;
}

uint32_t DSV::GetNextIndexGPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("DSV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexGPU_++;
}

} // namespace KashipanEngine