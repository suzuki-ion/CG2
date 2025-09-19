#include <cassert>

#include "RTV.h"
#include "Base/DirectXCommon.h"
#include "Common/Logs.h"

namespace KashipanEngine {
bool RTV::isInitialized_ = false;
DirectXCommon *RTV::dxCommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTV::descriptorHeap_ = nullptr;
uint32_t RTV::nextIndexCPU_ = 0;
uint32_t RTV::nextIndexGPU_ = 0;

void RTV::Initialize(DirectXCommon *dxCommon, const std::source_location &location) {
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
    // RTV用のヒープを生成
    //==================================================
    
    dxCommon_->CreateDescriptorHeap(
        descriptorHeap_,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        numDescriptors_,
        false
    );

    // 初期化完了のログを出力
    if (!isInitialized_) {
        LogSimple("Complete Initialize RTV.", kLogLevelFlagInfo);
        // 初期化済みフラグを立てる
        isInitialized_ = true;
    } else {
        LogSimple("Reinitialize RTV.", kLogLevelFlagInfo);
    }
}

void RTV::Finalize(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    
    // ディスクリプタヒープの解放
    descriptorHeap_.Reset();

    // 終了処理完了のログを出力
    LogSimple("Complete Finalize RTV.", kLogLevelFlagInfo);
}

ID3D12DescriptorHeap *RTV::GetDescriptorHeap(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    // ディスクリプタヒープを返す
    return descriptorHeap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RTV::GetCPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * nextIndexCPU_;
    nextIndexCPU_++;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE RTV::GetGPUDescriptorHandle(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * nextIndexGPU_;
    nextIndexGPU_++;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTV::GetCPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * index;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE RTV::GetGPUDescriptorHandle(const uint32_t index, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
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
    handle.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * index;
    return handle;
}

uint32_t RTV::GetNextIndexCPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexCPU_;
}

uint32_t RTV::GetNextIndexGPU(const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("RTV is not initialized.", kLogLevelFlagError);
        assert(false);
    }
    return nextIndexGPU_;
}

} // namespace KashipanEngine