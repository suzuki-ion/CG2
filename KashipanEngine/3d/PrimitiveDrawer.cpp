#include "PrimitiveDrawer.h"
#include "Common/VertexData.h"
#include "Common/Logs.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <d3d12shader.h>

#pragma comment(lib, "dxcompiler.lib")

namespace KashipanEngine {
bool PrimitiveDrawer::isInitialized_ = false;
DirectXCommon *PrimitiveDrawer::dxCommon_ = nullptr;

void PrimitiveDrawer::Initialize(DirectXCommon *dxCommon, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (isInitialized_) {
        Log("PrimitiveDrawer is already initialized.", kLogLevelFlagError);
        assert(false);
    }

    // nullチェック
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }

    // 引数をメンバ変数に格納
    dxCommon_ = dxCommon;
    // 初期化済みフラグを立てる
    isInitialized_ = true;

    // 初期化完了のログを出力
    LogSimple("PrimitiveDrawer Initialized.");
    LogNewLine();
}

Microsoft::WRL::ComPtr<ID3D12Resource> PrimitiveDrawer::CreateBufferResources(UINT64 size, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("PrimitiveDrawer is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // ヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
    // リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size; // リソースのサイズ
    // バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // リソースの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    // リソースの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // ログに生成したリソースのサイズを出力
    LogSimple(std::format("CreateBufferResources, size:{}", size));
    return resource;
}

PrimitiveDrawer::IntermediateMesh PrimitiveDrawer::CreateIntermediateMesh(UINT vertexCount, UINT indexCount, unsigned long long vertexStride, const std::source_location &location) {
    // 呼び出された場所のログを出力
    Log(location);

    // 頂点バッファの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer =
        CreateBufferResources(static_cast<UINT>(vertexStride) * vertexCount);
    // インデックスバッファの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer =
        CreateBufferResources(sizeof(uint32_t) * indexCount);

    //==================================================
    // 頂点バッファの設定
    //==================================================

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // リソースの先頭アドレスから使う
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    // 使用するリソースのサイズ
    vertexBufferView.SizeInBytes = static_cast<UINT>(vertexStride) * vertexCount;
    // 1頂点あたりのサイズ
    vertexBufferView.StrideInBytes = static_cast<UINT>(vertexStride);

    //==================================================
    // インデックスバッファの設定
    //==================================================

    // インデックスバッファビューを作成する
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    // リソースの先頭アドレスから使う
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    // 使用するリソースのサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * indexCount;
    // フォーマット
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    // 中間メッシュを返す
    IntermediateMesh intermediateMesh;
    intermediateMesh.vertexBuffer = vertexBuffer;
    intermediateMesh.indexBuffer = indexBuffer;
    intermediateMesh.vertexBufferView = vertexBufferView;
    intermediateMesh.indexBufferView = indexBufferView;

    // ログに生成したメッシュの頂点数とインデックス数を出力
    LogSimple(std::format("CreateMesh, vertexCount:{}, indexCount:{}", vertexCount, indexCount));

    return intermediateMesh;
}

} // namespace KashipanEngine