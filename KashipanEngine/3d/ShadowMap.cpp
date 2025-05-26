#include <cassert>

#include "ShadowMap.h"
#include "DirectionalLight.h"

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Common/Logs.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"

#include "Objects/Object.h"
#include "Objects/Plane.h"
#include "Objects/Tetrahedron.h"
#include "Objects/Sphere.h"
#include "Objects/Triangle.h"
#include "Objects/Sprite.h"
#include "Objects/Billboard.h"

namespace KashipanEngine {

ShadowMap::ShadowMap(DirectXCommon *dxCommon) {
    // nullチェック
    if (!dxCommon) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }

    // 引数をメンバ変数に格納
    dxCommon_ = dxCommon;

    // シャドウマップのリソースを作成
    resource_ = dxCommon_->CreateDepthStencilTextureResource(
        1024,
        1024,
        DXGI_FORMAT_D32_FLOAT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );

    // シャドウマップ用のDSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC shadowMapDesc{};
    shadowMapDesc.Format = DXGI_FORMAT_D32_FLOAT;
    shadowMapDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    shadowMapDesc.Flags = D3D12_DSV_FLAG_NONE;
    shadowMapDesc.Texture2D.MipSlice = 0;
    dxCommon_->GetDevice()->CreateDepthStencilView(
        resource_.Get(),
        &shadowMapDesc,
        DSV::GetCPUDescriptorHandle(1)
    );

    // シャドウマップ用のDSVを作成
    handle_ = DSV::GetCPUDescriptorHandle(1);
    // シャドウマップ用のパイプラインセットを作成
    pipelineSet_ = PrimitiveDrawer::CreateShadowMapPipeline();

    // ビューポートの設定
    viewport_.Width = 1024.0f;
    viewport_.Height = 1024.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;

    // シザー矩形の設定
    scissorRect_.left = 0;
    scissorRect_.top = 0;
    scissorRect_.right = 1024;
    scissorRect_.bottom = 1024;

    // 初期化完了のログを出力
    Log("ShadowMap Initialized.");
}

void ShadowMap::PreDraw() {
    // バリアの設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = dxCommon_->GetCurrentBarrierState();
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dxCommon_->SetBarrier(barrier);

    // シャドウマップのクリア
    dxCommon_->GetCommandList()->ClearDepthStencilView(
        handle_,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr
    );
    // シャドウマップ用のレンダーターゲットの設定
    dxCommon_->GetCommandList()->OMSetRenderTargets(
        0,
        nullptr,
        false,
        &handle_
    );

    // コマンドを積む
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_);         // ビューポートを設定
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);   // シザー矩形を設定
    // 形状を設定
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // シグネチャを設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet_.rootSignature.Get());
    // パイプラインを設定
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet_.pipelineState.Get());
    // 光源のビュープロジェクション行列を設定
    static auto lightViewProjectionResource = PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));
    static Matrix4x4 *lightViewProjectionData = nullptr;
    // シャドウマップ設定用のマップを取得
    lightViewProjectionResource->Map(0, nullptr, reinterpret_cast<void **>(&lightViewProjectionData));
    // 光源のビュープロジェクション行列を設定
    *lightViewProjectionData = directionalLight_->viewProjectionMatrix;
    // CBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, lightViewProjectionResource->GetGPUVirtualAddress());
}

void ShadowMap::PostDraw() {

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = dxCommon_->GetCurrentBarrierState();
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_READ;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dxCommon_->SetBarrier(barrier);
}

void ShadowMap::Draw(std::vector<Object *> &objects) {
    // 描画処理
    for (auto object : objects) {
        // 共通設定
        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &object->mesh->vertexBufferView);
        dxCommon_->GetCommandList()->IASetIndexBuffer(&object->mesh->indexBufferView);
        object->worldMatrix.MakeAffine(
            object->transform.scale,
            object->transform.rotate,
            object->transform.translate
        );
        object->transformationMatrixMap->wvp = directionalLight_->viewProjectionMatrix * object->worldMatrix;
        object->transformationMatrixMap->world = object->worldMatrix;
        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, object->transformationMatrixResource->GetGPUVirtualAddress());
        // 個々のオブジェクトの描画
        if (dynamic_cast<Triangle *>(object)) {
            dxCommon_->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0);
        } else if (dynamic_cast<Plane *>(object)) {
            dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
        } else if (dynamic_cast<Sphere *>(object)) {
            dxCommon_->GetCommandList()->DrawIndexedInstanced(12, 1, 0, 0, 0);
        } else if (dynamic_cast<Tetrahedron *>(object)) {
            dxCommon_->GetCommandList()->DrawIndexedInstanced(12, 1, 0, 0, 0);
        }
    }
}

void ShadowMap::SetLight(DirectionalLight *light) {
    directionalLight_ = light;
}

} // namespace KashipanEngine