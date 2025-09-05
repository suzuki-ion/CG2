#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Texture.h"
#include "PipeLineManager.h"
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

// レンダリングパイプライン名でのソート用関数
bool ComparePipelineNameObject(const Renderer::ObjectState &a, const Renderer::ObjectState &b) {
    return a.pipeLineName < b.pipeLineName;
}
// レンダリングパイプライン名でのソート用関数
bool ComparePipelineNameLine(const Renderer::LineState &a, const Renderer::LineState &b) {
    return a.pipeLineName < b.pipeLineName;
}

} // namespace

Renderer::Renderer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, PipeLineManager *pipeLineManager) {
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
    pipeLineManager_ = pipeLineManager;

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
    // 球面座標系に設定
    sDebugCamera->SetCoordinateSystem(Camera::CoordinateSystem::kSpherical);

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
    // 平行光源をリセット
    directionalLight_ = nullptr;

    // 2D用のプロジェクション行列を設定
    projectionMatrix2D_ = MakeOrthographicMatrix(
        0.0f,
        0.0f,
        //static_cast<float>(winApp_->GetClientWidth()),
        //static_cast<float>(winApp_->GetClientHeight()),
        1920.0f,
        1080.0f,
        0.0f,
        100.0f
    );
    static ID3D12DescriptorHeap *descriptorHeaps[] = { SRV::GetDescriptorHeap() };
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // デバッグカメラが有効ならデバッグカメラの処理
    if (isUseDebugCamera_) {
        sDebugCamera->MoveToMouse(0.1f, 0.01f, 0.1f);
    }
}

void Renderer::PostDraw() {
    // 描画オブジェクトをパイプライン名でソート
    //std::sort(drawObjects_.begin(), drawObjects_.end(), ComparePipelineNameObject);
    //std::sort(drawAlphaObjects_.begin(), drawAlphaObjects_.end(), ComparePipelineNameObject);
    //std::sort(draw2DObjects_.begin(), draw2DObjects_.end(), ComparePipelineNameObject);
    //std::sort(drawLines_.begin(), drawLines_.end(), ComparePipelineNameLine);

    // 光源が設定されていなければデフォルトの光源を設定
    if (directionalLight_ == nullptr) {
        directionalLight_ = &sDefaultDirectionalLight;
    }

    // 線の描画
    for (auto &line : drawLines_) {
        DrawLine(&line);
    }

    pipeLineManager_->ResetCurrentPipeLine();
    pipeLineManager_->SetCommandListPipeLine("Object3d.Solid.BlendNormal");

    // 平行光源の設定
    SetLightBuffer(directionalLight_);
    // 通常のオブジェクトの描画
    DrawCommon(drawObjects_);
    // 半透明オブジェクトの描画
    DrawCommon(drawAlphaObjects_);
    // 2Dオブジェクトの描画
    DrawCommon(draw2DObjects_);

    // 描画オブジェクトのクリア
    drawObjects_.clear();
    drawAlphaObjects_.clear();
    draw2DObjects_.clear();
    // グリッドラインのクリア
    drawLines_.clear();
}

void Renderer::ToggleDebugCamera() {
    // デバッグカメラのトグル
    isUseDebugCamera_ = !isUseDebugCamera_;
}

void Renderer::SetCamera(Camera *camera) {
    sCameraPtr = camera;
}

void Renderer::DrawSetLine(LineState &lineState) {
    // ラインを追加
    drawLines_.push_back(lineState);
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
    pipeLineManager_->SetCommandListPipeLine(objectState->pipeLineName);

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

    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, Texture::GetTexture(objectState->useTextureIndex).srvHandleGPU);

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

void Renderer::DrawLine(LineState *lineState) {
    pipeLineManager_->SetCommandListPipeLine(lineState->pipeLineName);

    if (isUseDebugCamera_) {
        sDebugCamera->SetWorldMatrix(Matrix4x4::Identity());
        sDebugCamera->CalculateMatrix();
        lineState->transformationMatrixMap->wvp = sDebugCamera->GetWVPMatrix();
        lineState->transformationMatrixMap->viewportInverse = sDebugCamera->GetViewportMatrix();
    } else {
        sCameraPtr->SetWorldMatrix(Matrix4x4::Identity());
        sCameraPtr->CalculateMatrix();
        lineState->transformationMatrixMap->wvp = sCameraPtr->GetWVPMatrix();
        lineState->transformationMatrixMap->viewportInverse = sCameraPtr->GetViewportMatrix().Inverse();
    }

    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, lineState->transformationMatrixResource->GetGPUVirtualAddress());
    // LineOption用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, lineState->lineOptionResource->GetGPUVirtualAddress());

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &lineState->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&lineState->mesh->indexBufferView);

    // 描画コマンドを発行
    if (lineState->indexCount > 0) {
        dxCommon_->GetCommandList()->DrawIndexedInstanced(lineState->indexCount, 1, 0, 0, 0);
    } else {
        dxCommon_->GetCommandList()->DrawInstanced(lineState->vertexCount, 1, 0, 0);
    }
}

} // namespace KashipanEngine