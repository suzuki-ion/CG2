#include <cmath>
#include <algorithm>

#include "Drawer.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Math/RenderingPipeline.h"
#include "3d/DirectionalLight.h"
#include "3d/ShadowMap.h"

#include "Common/Logs.h"
#include "Common/ConvertColor.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"

#include "Objects/Object.h"
#include "Objects/Triangle.h"
#include "Objects/Sprite.h"
#include "Objects/Sphere.h"
#include "Objects/Billboard.h"
#include "Objects/Model.h"
#include "Objects/Tetrahedron.h"
#include "Objects/Plane.h"

#define M_PI (4.0f * std::atanf(1.0f))

namespace KashipanEngine {

namespace {

/// @brief デバッグカメラの初期化
std::unique_ptr<Camera> sDebugCamera;
/// @brief デフォルトの平行光源
DirectionalLight sDefaultDirectionalLight = {
    { 255.0f, 255.0f, 255.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    1.0f
};

bool ZSort(Object *a, Object *b) {
    // カメラからの距離でソート
    return a->transform.translate.Distance(a->camera->GetTranslate()) <
        b->transform.translate.Distance(b->camera->GetTranslate());
}

} // namespace

Drawer::Drawer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, TextureManager *textureManager) {
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
    if (!textureManager) {
        Log("textureManager is null.", kLogLevelFlagError);
        assert(false);
    }
    // 各クラスへのポインタを設定
    winApp_ = winApp;
    dxCommon_ = dxCommon;
    imguiManager_ = imguiManager;
    textureManager_ = textureManager;

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
        winApp_,
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
    Log("Drawer Initialized.");
    LogNewLine();
}

Drawer::~Drawer() {
    // 各クラスのポインタをnullにする
    winApp_ = nullptr;
    dxCommon_ = nullptr;
    imguiManager_ = nullptr;
    // デバッグカメラ解放
    sDebugCamera.reset();
    // 終了処理完了のログを出力
    Log("Drawer Finalized.");
}

void Drawer::PreDraw() {
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

void Drawer::PostDraw() {
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

void Drawer::ToggleDebugCamera() {
    // デバッグカメラのトグル
    isUseDebugCamera_ = !isUseDebugCamera_;
}

void Drawer::DrawSet(Object *object) {
    // カメラが設定されていないものは2Dオブジェクトとして扱う
    if (object->camera == nullptr) {
        draw2DObjects_.push_back(object);
    } else {
        // カメラが設定されているものは3Dオブジェクトとして扱う
        if (object->material.color.w < 255.0f) {
            // アルファ値が255.0f未満のものは半透明オブジェクトとして扱う
            drawAlphaObjects_.push_back(object);
        } else {
            drawObjects_.push_back(object);
        }
    }
}

//void Drawer::DrawShadowMap() {
//    //--------- 未完成 ---------//
//    return;
//
//    // シャドウマップの描画
//    if (shadowMap_) {
//        // シャドウマップの描画を開始
//        shadowMap_->PreDraw();
//        // シャドウマップの描画を実行
//        shadowMap_->Draw(drawObjects_);
//        shadowMap_->Draw(drawAlphaObjects_);
//        // シャドウマップの描画を終了
//        shadowMap_->PostDraw();
//    }
//}

void Drawer::SetLightBuffer(DirectionalLight *light) {
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

void Drawer::Draw(Triangle *triangle) {
    // 法線を設定
    static Vector3 position[3];
    if (triangle->camera == nullptr || triangle->material.enableLighting == false) {
        for (int i = 0; i < 3; i++) {
            triangle->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (triangle->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 3; i++) {
                triangle->mesh->vertexBufferMap[i].normal =
                    Vector3(triangle->mesh->vertexBufferMap[i].position);
            }
        } else if (triangle->normalType == kNormalTypeFace) {
            position[0] = Vector3(triangle->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(triangle->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(triangle->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 3; i++) {
                triangle->mesh->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画処理
    DrawCommon(triangle);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Drawer::Draw(Sprite *sprite) {
    // 法線を設定
    static Vector3 position[3];
    if (sprite->camera == nullptr || sprite->material.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            sprite->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (sprite->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                sprite->mesh->vertexBufferMap[i].normal =
                    Vector3(sprite->mesh->vertexBufferMap[i].position);
            }
        } else if (sprite->normalType == kNormalTypeFace) {
            position[0] = Vector3(sprite->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(sprite->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(sprite->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                sprite->mesh->vertexBufferMap[sprite->mesh->indexBufferMap[i]].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画処理
    DrawCommon(sprite);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Drawer::Draw(Sphere *sphere) {
    static Vector3 position[3];
    // 緯度の方向に分割
    for (uint32_t latIndex = 0; latIndex < sphere->kSubdivision; latIndex++) {
        // 経度の方向に分割
        for (uint32_t lonIndex = 0; lonIndex < sphere->kSubdivision; lonIndex++) {
            // 頂点位置を計算
            const uint32_t vertexIndex = (latIndex * sphere->kSubdivision + lonIndex) * 4;

            //--------- 法線を設定 ---------//

            if (sphere->camera == nullptr || sphere->material.enableLighting == false) {
                for (int i = 0; i < 4; i++) {
                    sphere->mesh->vertexBufferMap[vertexIndex + i].normal = { 0.0f, 0.0f, -1.0f };
                }
                continue;
            }
            if (sphere->normalType == kNormalTypeVertex) {
                sphere->mesh->vertexBufferMap[vertexIndex + 0].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 0].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 1].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 2].normal = 
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 3].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 3].position);

            } else if (sphere->normalType == kNormalTypeFace) {
                position[0] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 0].position);
                position[1] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
                position[2] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
                for (int i = 0; i < 4; i++) {
                    sphere->mesh->vertexBufferMap[vertexIndex + i].normal =
                        (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
                }
            }
        }
    }
    // 面法線の場合、真下の面の法線だけ別で計算する
    if (sphere->normalType == kNormalTypeFace) {
        for (uint32_t i = 0; i < sphere->kSubdivision; i++) {
            const uint32_t vertexIndex = i * 4;
            position[0] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
            position[1] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 3].position);
            position[2] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
            for (int j = 0; j < 4; j++) {
                sphere->mesh->vertexBufferMap[vertexIndex + j].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画処理
    DrawCommon(sphere);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(sphere->kIndexCount, 1, 0, 0, 0);
}

void Drawer::Draw(BillBoard *billboard) {
    // 向きをカメラの向きに合わせる
    billboard->transform.rotate = billboard->camera->GetRotate();
    // 法線を設定
    static Vector3 position[3];
    if (billboard->material.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            billboard->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (billboard->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                billboard->mesh->vertexBufferMap[i].normal =
                    Vector3(billboard->mesh->vertexBufferMap[i].position);
            }
        } else if (billboard->normalType == kNormalTypeFace) {
            position[0] = Vector3(billboard->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(billboard->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(billboard->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                billboard->mesh->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画処理
    DrawCommon(billboard);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Drawer::Draw(ModelData *model) {
    // 描画処理
    DrawCommon(model);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(model->indexCount, 1, 0, 0, 0);
}

void Drawer::Draw(Tetrahedron *tetrahedron) {
    // 法線を設定
    static Vector3 position[3];
    if (tetrahedron->camera == nullptr || tetrahedron->material.enableLighting == false) {
        for (int i = 0; i < 12; i++) {
            tetrahedron->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (tetrahedron->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 12; i++) {
                tetrahedron->mesh->vertexBufferMap[i].normal =
                    Vector3(tetrahedron->mesh->vertexBufferMap[i].position);
            }
        } else if (tetrahedron->normalType == kNormalTypeFace) {
            for (int i = 0; i < 4; i++) {
                position[0] = Vector3(tetrahedron->mesh->vertexBufferMap[i * 3 + 0].position);
                position[1] = Vector3(tetrahedron->mesh->vertexBufferMap[i * 3 + 1].position);
                position[2] = Vector3(tetrahedron->mesh->vertexBufferMap[i * 3 + 2].position);
                for (int j = 0; j < 3; j++) {
                    tetrahedron->mesh->vertexBufferMap[i * 3 + j].normal =
                        (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
                }
            }
        }
    }
    // 描画処理
    DrawCommon(tetrahedron);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(12, 1, 0, 0, 0);
}

void Drawer::Draw(Plane *plane) {
    // 法線を設定
    static Vector3 position[3];
    if (plane->camera == nullptr || plane->material.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            plane->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (plane->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                plane->mesh->vertexBufferMap[i].normal =
                    Vector3(plane->mesh->vertexBufferMap[i].position);
            }
        } else if (plane->normalType == kNormalTypeFace) {
            position[0] = Vector3(plane->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(plane->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(plane->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                plane->mesh->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
    // 描画処理
    DrawCommon(plane);
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

//void Drawer::DrawShadowMapCommon(std::vector<Object *> &objects) {
//    // 描画処理
//    for (auto object : objects) {
//        // 共通設定
//        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &object->mesh->vertexBufferView);
//        dxCommon_->GetCommandList()->IASetIndexBuffer(&object->mesh->indexBufferView);
//        object->worldMatrix.MakeAffine(
//            object->transform.scale,
//            object->transform.rotate,
//            object->transform.translate
//        );
//        if (isUseDebugCamera_) {
//            sDebugCamera->SetWorldMatrix(object->worldMatrix);
//            sDebugCamera->CalculateMatrix();
//            object->transformationMatrixMap->wvp = sDebugCamera->GetWVPMatrix();
//        } else {
//            object->camera->SetWorldMatrix(object->worldMatrix);
//            object->camera->CalculateMatrix();
//            object->transformationMatrixMap->wvp = object->camera->GetWVPMatrix();
//        }
//        object->transformationMatrixMap->world = object->worldMatrix;
//        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, object->transformationMatrixResource->GetGPUVirtualAddress());
//        
//        // 個々のオブジェクトの描画
//        if (dynamic_cast<Triangle *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0);
//        } else if (dynamic_cast<Sprite *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
//        } else if (dynamic_cast<Sphere *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(dynamic_cast<Sphere *>(object)->kVertexCount, 1, 0, 0, 0);
//        } else if (dynamic_cast<BillBoard *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
//        } else if (dynamic_cast<ModelData *>(object)) {
//            dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(dynamic_cast<ModelData *>(object)->vertices.size()), 1, 0, 0);
//        } else if (dynamic_cast<Tetrahedron *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(12, 1, 0, 0, 0);
//        } else if (dynamic_cast<Plane *>(object)) {
//            dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
//        }
//    }
//}

void Drawer::DrawCommon(std::vector<Object *> &objects) {
    // 描画処理
    for (auto object : objects) {
        if (dynamic_cast<Triangle *>(object)) {
            Draw(static_cast<Triangle *>(object));
        } else if (dynamic_cast<Sprite *>(object)) {
            Draw(static_cast<Sprite *>(object));
        } else if (dynamic_cast<Sphere *>(object)) {
            Draw(static_cast<Sphere *>(object));
        } else if (dynamic_cast<BillBoard *>(object)) {
            Draw(static_cast<BillBoard *>(object));
        } else if (dynamic_cast<ModelData *>(object)) {
            Draw(static_cast<ModelData *>(object));
        } else if (dynamic_cast<Tetrahedron *>(object)) {
            Draw(static_cast<Tetrahedron *>(object));
        } else if (dynamic_cast<Plane *>(object)) {
            Draw(static_cast<Plane *>(object));
        }
    }
}

void Drawer::DrawCommon(Object *object) {
    // マテリアルを設定
    object->materialMap->color = ConvertColor(object->material.color);
    object->materialMap->enableLighting = object->material.enableLighting;
    object->materialMap->uvTransform.MakeAffine(
        object->uvTransform.scale,
        object->uvTransform.rotate,
        object->uvTransform.translate
    );

    // 行列を計算
    object->worldMatrix.MakeAffine(
        object->transform.scale,
        object->transform.rotate,
        object->transform.translate
    );

    // Cameraがnullptrの場合は2D描画
    if (object->camera == nullptr) {
        wvpMatrix2D_ = object->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        object->transformationMatrixMap->wvp = wvpMatrix2D_;
        object->transformationMatrixMap->world = object->worldMatrix;
    } else {
        if (isUseDebugCamera_) {
            sDebugCamera->SetWorldMatrix(object->worldMatrix);
            sDebugCamera->CalculateMatrix();
            object->transformationMatrixMap->wvp = sDebugCamera->GetWVPMatrix();
        } else {
            object->camera->SetWorldMatrix(object->worldMatrix);
            object->camera->CalculateMatrix();
            object->transformationMatrixMap->wvp = object->camera->GetWVPMatrix();
        }
        object->transformationMatrixMap->world = object->worldMatrix;
    }

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (object->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(object->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(0).srvHandleGPU);
    }

    // 形状を設定
    /*if (object->fillMode == kFillModeSolid) {
        dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    } else if (object->fillMode == kFillModeWireframe) {
        dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    }*/
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // ルートシグネチャを設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet_[object->fillMode][blendMode_].rootSignature.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet_[object->fillMode][blendMode_].pipelineState.Get());
    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &object->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&object->mesh->indexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, object->materialResource->GetGPUVirtualAddress());

    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, object->transformationMatrixResource->GetGPUVirtualAddress());
}

} // namespace KashipanEngine