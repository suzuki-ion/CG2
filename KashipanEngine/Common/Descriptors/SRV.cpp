#include <cassert>

#include "SRV.h"
#include "Base/DirectXCommon.h"
#include "Common/Logs.h"

namespace KashipanEngine {
bool SRV::isInitialized_ = false;
DirectXCommon *SRV::dxCommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRV::descriptorHeap_ = nullptr;
uint32_t SRV::nextIndexCPU_ = 0;
uint32_t SRV::nextIndexGPU_ = 0;

void SRV::Initialize(DirectXCommon *dxCommon, const std::source_location &location) {
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
    // SRV用のヒープを生成
    //==================================================

    dxCommon_->CreateDescriptorHeap(
        descriptorHeap_,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        numDescriptors_,
        true
    );

    // 初期化完了のログを出力
    if (!isInitialized_) {
        LogSimple("Complete Initialize SRV.", kLogLevelFlagInfo);
        // 初期化済みフラグを立てる
        isInitialized_ = true;
    } else {
        LogSimple("Reinitialize SRV.", kLogLevelFlagInfo);
    }
}

void SRV::Finalize(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // ディスクリプタヒープの解放
    descriptorHeap_.Reset();

    // 終了処理完了のログを出力
    LogSimple("Complete Finalize SRV.", kLogLevelFlagInfo);
}

ID3D12DescriptorHeap *SRV::GetDescriptorHeap(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // ディスクリプタヒープを返す
    return descriptorHeap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SRV::GetCPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
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

D3D12_GPU_DESCRIPTOR_HANDLE SRV::GetGPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
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

uint32_t SRV::GetNextIndexCPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexCPU_;
}

uint32_t SRV::GetNextIndexGPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("SRV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexGPU_;
}

} // namespace KashipanEngine