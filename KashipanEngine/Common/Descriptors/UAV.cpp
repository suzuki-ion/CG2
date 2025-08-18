#include <cassert>

#include "UAV.h"
#include "Base/DirectXCommon.h"
#include "Common/Logs.h"

namespace KashipanEngine {
bool UAV::isInitialized_ = false;
DirectXCommon *UAV::dxCommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> UAV::descriptorHeap_ = nullptr;
uint32_t UAV::nextIndexCPU_ = 0;
uint32_t UAV::nextIndexGPU_ = 0;

void UAV::Initialize(DirectXCommon *dxCommon, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);

    // 初回だけnullチェックと変数の初期化をする
    if (!isInitialized_) {
        // nullチェック
        if (dxCommon == nullptr) {
            Log("dxCommon is null.", kLogLevelFlagError);
            assert(false);
        }

        // 引数をメンバ変数に格納
        dxCommon_ = dxCommon;
    }

    //==================================================
    // UAV用のヒープを生成
    //==================================================

    dxCommon_->CreateDescriptorHeap(
        descriptorHeap_,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        numDescriptors_,
        true
    );

    // 初期化完了のログを出力
    if (!isInitialized_) {
        LogSimple("Complete Initialize UAV.", kLogLevelFlagInfo);
        // 初期化済みフラグを立てる
        isInitialized_ = true;
    } else {
        LogSimple("Reinitialize UAV.", kLogLevelFlagInfo);
    }
}

void UAV::Finalize(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // ディスクリプタヒープの解放
    descriptorHeap_.Reset();

    // 終了処理完了のログを出力
    LogSimple("Complete Finalize UAV.", kLogLevelFlagInfo);
}

ID3D12DescriptorHeap *UAV::GetDescriptorHeap(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // ディスクリプタヒープを返す
    return descriptorHeap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE UAV::GetCPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * nextIndexCPU_;
    nextIndexCPU_++;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE UAV::GetGPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * nextIndexGPU_;
    nextIndexGPU_++;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE UAV::GetCPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * index;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE UAV::GetGPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * index;
    return handle;
}

uint32_t UAV::GetNextIndexCPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexCPU_;
}

uint32_t UAV::GetNextIndexGPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("UAV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexGPU_;
}

} // namespace KashipanEngine