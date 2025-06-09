#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Texture.h"
#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Math/RenderingPipeline.h"
#include "3d/DirectionalLight.h"

#include "Common/Logs.h"
#include "Common/ConvertColor.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"

#define M_PI (4.0f * std::atanf(1.0f))

namespace KashipanEngine {

namespace {

/// @brief カメラへのポインタ
Camera *sCameraPtr = nullptr;
/// @brief デバッグカメラ
std::unique_ptr<Camera> sDebugCamera;
/// @brief デフォルトの平行光源
DirectionalLight sDefaultDirectionalLight = {
    { 255.0f, 255.0f, 255.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    1.0f
};

//bool ZSort(Object *a, Object *b) {
//    // カメラからの距離でソート
//    return a->transform.translate.Distance(a->camera->GetTranslate()) <
//        b->transform.translate.Distance(b->camera->GetTranslate());
//}

} // namespace

Renderer::Renderer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager) {
    // nullチェック
    if (!winApp) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    if (!dxCommon) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }
    if (!imguiManager) {
        Log("imguiManager is null.", kLogLevelFlagError);
        assert(false);
    }
    // 各クラスへのポインタを設定
    winApp_ = winApp;
    dxCommon_ = dxCommon;
    imguiManager_ = imguiManager;

    // 2D描画用の行列を初期化
    viewMatrix2D_.MakeIdentity();
    projectionMatrix2D_ = MakeOrthographicMatrix(
        0.0f,
        0.0f,
        static_cast<float>(winApp_->GetClientWidth()),
        static_cast<float>(winApp_->GetClientHeight()),
        0.0f,
        100.0f
    );

    // デバッグカメラの初期化
    sDebugCamera = std::make_unique<Camera>(
        Vector3(0.0f, 0.0f, -5.0f),
        Vector3(0.0f, 0.0f, 0.0f),
        Vector3(1.0f, 1.0f, 1.0f)
    );

    //==================================================
    // パイプラインセットの初期化
    //==================================================

    pipelineSet_[kFillModeSolid][kBlendModeNone] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeNone);
    pipelineSet_[kFillModeSolid][kBlendModeNormal] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeNormal);
    pipelineSet_[kFillModeSolid][kBlendModeAdd] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeAdd);
    pipelineSet_[kFillModeSolid][kBlendModeSubtract] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeSubtract);
    pipelineSet_[kFillModeSolid][kBlendModeMultiply] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeMultiply);
    pipelineSet_[kFillModeSolid][kBlendModeScreen] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeScreen);
    pipelineSet_[kFillModeSolid][kBlendModeExclusion] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeExclusion);

    pipelineSet_[kFillModeWireframe][kBlendModeNone] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeNone);
    pipelineSet_[kFillModeWireframe][kBlendModeNormal] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeNormal);
    pipelineSet_[kFillModeWireframe][kBlendModeAdd] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeAdd);
    pipelineSet_[kFillModeWireframe][kBlendModeSubtract] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeSubtract);
    pipelineSet_[kFillModeWireframe][kBlendModeMultiply] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeMultiply);
    pipelineSet_[kFillModeWireframe][kBlendModeScreen] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeScreen);
    pipelineSet_[kFillModeWireframe][kBlendModeExclusion] =
        PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeExclusion);

    // 初期化完了のログを出力
    Log("Renderer Initialized.");
    LogNewLine();
}

Renderer::~Renderer() {
    // 各クラスのポインタをnullにする
    winApp_ = nullptr;
    dxCommon_ = nullptr;
    imguiManager_ = nullptr;
    // デバッグカメラ解放
    sDebugCamera.reset();
    // 終了処理完了のログを出力
    Log("Renderer Finalized.");
}

void Renderer::PreDraw() {
    dxCommon_->PreDraw();
    dxCommon_->ClearDepthStencil();
    imguiManager_->BeginFrame();

    // 平行光源をリセット
    directionalLight_ = nullptr;
    // ブレンドモードを初期化
    blendMode_ = kBlendModeNormal;
    // 描画オブジェクトのクリア
    drawObjects_.clear();
    drawAlphaObjects_.clear();
    draw2DObjects_.clear();

    // ビューポートの設定
    viewport_.Width = static_cast<FLOAT>(winApp_->GetClientWidth());
    viewport_.Height = static_cast<FLOAT>(winApp_->GetClientHeight());
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;

    // シザー矩形の設定
    scissorRect_.left = 0;
    scissorRect_.right = static_cast<LONG>(winApp_->GetClientWidth());
    scissorRect_.top = 0;
    scissorRect_.bottom = static_cast<LONG>(winApp_->GetClientHeight());

    static ID3D12DescriptorHeap *descriptorHeaps[] = { SRV::GetDescriptorHeap() };
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // コマンドを積む
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_);         // ビューポートを設定
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);   // シザー矩形を設定
    // 形状を設定
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // シグネチャを設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet_[kFillModeSolid][blendMode_].rootSignature.Get());
    // パイプラインを設定
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet_[kFillModeSolid][blendMode_].pipelineState.Get());

    // デバッグカメラが有効ならデバッグカメラの処理
    if (isUseDebugCamera_) {
        sDebugCamera->MoveToMouse(0.01f, 0.01f, 0.001f);
    }
}

void Renderer::PostDraw() {
    // 光源が設定されていなければデフォルトの光源を設定
    if (directionalLight_ == nullptr) {
        directionalLight_ = &sDefaultDirectionalLight;
    }

    // 平行光源の設定
    SetLightBuffer(directionalLight_);

    // 通常のオブジェクトの描画
    DrawCommon(drawObjects_);
    // 半透明オブジェクトの描画
    DrawCommon(drawAlphaObjects_);
    // 2Dオブジェクトの描画
    DrawCommon(draw2DObjects_);

    imguiManager_->EndFrame();
    dxCommon_->PostDraw();
}

void Renderer::ToggleDebugCamera() {
    // デバッグカメラのトグル
    isUseDebugCamera_ = !isUseDebugCamera_;
}

void Renderer::SetCamera(Camera *camera) {
    sCameraPtr = camera;
}

void Renderer::DrawSet(const ObjectState &objectState, bool isUseCamera, bool isSemitransparent) {
    // カメラが設定されていないものは2Dオブジェクトとして扱う
    if (isUseCamera == false) {
        draw2DObjects_.push_back(objectState);
    } else {
        // カメラが設定されているものは3Dオブジェクトとして扱う
        if (isSemitransparent) {
            // 半透明オブジェクトとして扱う
            drawAlphaObjects_.push_back(objectState);
        } else {
            // 通常のオブジェクトとして扱う
            drawObjects_.push_back(objectState);
        }
    }
}

void Renderer::SetLightBuffer(DirectionalLight *light) {
    // 光源のリソースを生成
    static auto directionalLightResource = PrimitiveDrawer::CreateBufferResources(sizeof(DirectionalLight));
    // 光源設定用のマップ
    static DirectionalLight *directionalLightData = nullptr;

    // 光源設定用のマップを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void **>(&directionalLightData));
    // 光源のデータを設定
    directionalLightData->color = ConvertColor(light->color);
    directionalLightData->direction = light->direction;
    directionalLightData->intensity = light->intensity;
    // 光源のビューと射影行列
    directionalLightData->viewProjectionMatrix = light->viewProjectionMatrix;

    // CBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
}

void Renderer::DrawCommon(std::vector<ObjectState> &objects) {
    // 描画処理
    for (auto &object : objects) {
        DrawCommon(&object);
    }
}

void Renderer::DrawCommon(ObjectState *objectState) {
    // Cameraがnullptrの場合は2D描画
    if (objectState->isUseCamera == false) {
        wvpMatrix2D_ = *objectState->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        objectState->transformationMatrixMap->wvp = wvpMatrix2D_;
    } else {
        if (isUseDebugCamera_) {
            sDebugCamera->SetWorldMatrix(*objectState->worldMatrix);
            sDebugCamera->CalculateMatrix();
            objectState->transformationMatrixMap->wvp = sDebugCamera->GetWVPMatrix();
        } else {
            sCameraPtr->SetWorldMatrix(*objectState->worldMatrix);
            sCameraPtr->CalculateMatrix();
            objectState->transformationMatrixMap->wvp = sCameraPtr->GetWVPMatrix();
        }
    }

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (objectState->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, Texture::GetTexture(objectState->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, Texture::GetTexture(0).srvHandleGPU);
    }

    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // ルートシグネチャを設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet_[objectState->fillMode][blendMode_].rootSignature.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet_[objectState->fillMode][blendMode_].pipelineState.Get());
    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &objectState->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&objectState->mesh->indexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, objectState->materialResource->GetGPUVirtualAddress());
    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, objectState->transformationMatrixResource->GetGPUVirtualAddress());

    // 描画コマンドを発行
    if (objectState->indexCount > 0) {
        dxCommon_->GetCommandList()->DrawIndexedInstanced(objectState->indexCount, 1, 0, 0, 0);
    } else {
        dxCommon_->GetCommandList()->DrawInstanced(objectState->vertexCount, 1, 0, 0);
    }
}

} // namespace KashipanEngine