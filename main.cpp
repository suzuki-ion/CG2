#include <Engine.h>
#include <memory>
#include <imgui.h>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Drawer.h"
#include "Base/TextureManager.h"
#include "Base/Input.h"
#include "Base/Sound.h"

#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Common/ConvertColor.h"

#include "3d/DirectionalLight.h"
#include "Objects/Sphere.h"
#include "Objects/Model.h"
#include "Objects/Plane.h"

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("KashipanEngine", 1920, 1080, true);

    // WinAppクラスへのポインタ
    WinApp *winApp = myGameEngine->GetWinApp();
    winApp->SetSizeChangeMode(SizeChangeMode::kNormal);
    // DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon = myGameEngine->GetDxCommon();
    // 描画用クラスへのポインタ
    Drawer *drawer = myGameEngine->GetDrawer();
    // テクスチャ管理クラスへのポインタ
    TextureManager *textureManager = myGameEngine->GetTextureManager();

    // テクスチャを読み込む
    uint32_t textures[2];
    textures[0] = textureManager->Load("Resources/uvChecker.png");
    textures[1] = textureManager->Load("Resources/monsterBall.png");

    // ブレンドモード
    BlendMode blendMode = kBlendModeNormal;

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>(
        winApp,
        Vector3( 0.0f, 2.0f, -16.0f ),
        Vector3( 0.0f, 0.0f, 0.0f ),
        Vector3( 1.0f, 1.0f, 1.0f )
    );
    // デバッグカメラの有効化フラグ
    bool isUseDebugCamera = true;

    //==================================================
    // 背景の色
    //==================================================

    Vector4 clearColor = { 32.0f, 32.0f, 32.0f, 255.0f };

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight = {
        { 255.0f, 255.0f, 255.0f, 255.0f },
        { 0.5f, -0.75f, 0.5f },
        1.0f
    };

    //==================================================
    // 球体
    //==================================================

    Sphere sphere(16);
    sphere.transform.translate.y = 4.0f;
    sphere.radius = 1.0f;
    sphere.camera = camera.get();
    sphere.normalType = kNormalTypeFace;

    //==================================================
    // モデル
    //==================================================

    Model model("Resources", "multiMaterial.obj", textureManager);
    for (auto &modelData : model.models) {
        // 位置を設定
        modelData.transform.translate.y = 0.0f;
        // カメラを設定
        modelData.camera = camera.get();
    }

    //==================================================
    // 床用の板
    //==================================================

    Plane floor(camera.get());
    // 左前
    floor.mesh->vertexBufferMap[0].position = { -5.0f, 0.0f, -5.0f, 1.0f };
    floor.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 左奥
    floor.mesh->vertexBufferMap[1].position = { -5.0f, 0.0f, 5.0f, 1.0f };
    floor.mesh->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    // 右前
    floor.mesh->vertexBufferMap[2].position = { 5.0f, 0.0f, -5.0f, 1.0f };
    floor.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // 右奥
    floor.mesh->vertexBufferMap[3].position = { 5.0f, 0.0f, 5.0f, 1.0f };
    floor.mesh->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
    floor.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    floor.material.enableLighting = true;
    // テクスチャを設定
    floor.useTextureIndex = textures[0];
    // 法線の種類
    floor.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    floor.fillMode = kFillModeSolid;

    //==================================================
    // 音声
    //==================================================

    int soundIndex = Sound::Load("C:\\Windows\\Media\\chord.wav");
    float volume = 1.0f;
    float pitch = 0.0f;
    bool isPlay = false;
    bool isLoop = false;

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }
        drawer->PreDraw();
        Input::Update();

        //==================================================
        // 更新処理
        //==================================================

        // F3キーでデバッグカメラの有効化
        if (Input::IsKeyTrigger(DIK_F3)) {
            isUseDebugCamera = !isUseDebugCamera;
            drawer->ToggleDebugCamera();
        }

        // 1 ～ 6 でブレンドモードを変更
        if (Input::IsKeyTrigger(DIK_1)) {
            blendMode = kBlendModeNone;
        } else if (Input::IsKeyTrigger(DIK_2)) {
            blendMode = kBlendModeNormal;
        } else if (Input::IsKeyTrigger(DIK_3)) {
            blendMode = kBlendModeAdd;
        } else if (Input::IsKeyTrigger(DIK_4)) {
            blendMode = kBlendModeSubtract;
        } else if (Input::IsKeyTrigger(DIK_5)) {
            blendMode = kBlendModeMultiply;
        } else if (Input::IsKeyTrigger(DIK_6)) {
            blendMode = kBlendModeScreen;
        }

        ImGuiManager::Begin("オブジェクト");

        // FPSの表示
        ImGui::Text("FPS: %d", myGameEngine->GetFPS());
        // デルタタイムの表示
        ImGui::Text("DeltaTime: %.2f", myGameEngine->GetDeltaTime());

        // カメラ位置の表示
        ImGui::Text("カメラ位置: (%.2f, %.2f, %.2f)", camera->GetTranslate().x, camera->GetTranslate().y, camera->GetTranslate().z);
        // カメラの回転の表示
        ImGui::Text("カメラの回転: (%.2f, %.2f, %.2f)", camera->GetRotate().x, camera->GetRotate().y, camera->GetRotate().z);
        // ブレンドモードの表示
        ImGui::Text("ブレンドモード: %d", static_cast<int>(blendMode));

        // デバッグカメラの有効化
        if (ImGui::Checkbox("デバッグカメラ有効化", &isUseDebugCamera)) {
            drawer->ToggleDebugCamera();
        }

        // 背景色
        ImGui::DragFloat4("背景色", &clearColor.x, 1.0f, 0.0f, 255.0f);

        // 音声
        if (ImGui::TreeNode("音声")) {
            ImGui::DragFloat("音量", &volume, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("ピッチ", &pitch, 0.01f, -24.0f, 24.0f);
            ImGui::Checkbox("ループ再生", &isLoop);
            if (ImGui::Button("再生")) {
                Sound::Play(soundIndex, volume, pitch, isLoop);
            }
            if (ImGui::Button("停止")) {
                Sound::Stop(soundIndex);
            }
            ImGui::TreePop();
        }

        // 平行光源
        if (ImGui::TreeNode("平行光源")) {
            ImGui::DragFloat3("DirectionalLight Direction", &directionalLight.direction.x, 0.01f);
            ImGui::DragFloat4("DirectionalLight Color", &directionalLight.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::DragFloat("DirectionalLight Intensity", &directionalLight.intensity, 0.01f);
            ImGui::TreePop();
        }

        // モデル
        if (ImGui::TreeNode("モデル")) {
            // モデル内のモデルデータの選択
            static int modelIndex = 0;
            ImGui::Combo("モデルデータ", &modelIndex, {
                "モデルデータ0\0"
                "モデルデータ1\0"
                "モデルデータ2\0"
                "モデルデータ3\0"
                "モデルデータ4\0"
                "モデルデータ5\0"
                "モデルデータ6\0"
                "モデルデータ7\0"
                "モデルデータ8\0"
                "モデルデータ9\0"
                "モデルデータ10\0"
                "モデルデータ11\0"
                "モデルデータ12\0"
                "モデルデータ13\0"
                "モデルデータ14\0"
                "モデルデータ15\0"
                "モデルデータ16\0"
                "モデルデータ17\0"
                "モデルデータ18\0"
                "モデルデータ19\0"
                "モデルデータ20\0"
                "モデルデータ21\0"
                "モデルデータ22\0"
            }, static_cast<int>(model.models.size()));
            // モデルの選択が変更されたら、モデルを変更
            if (modelIndex >= 0 && modelIndex < model.models.size()) {
                // モデルの位置
                ImGui::DragFloat3("Model Translate", &model.models[modelIndex].transform.translate.x, 0.01f);
                // モデルの回転
                ImGui::DragFloat3("Model Rotate", &model.models[modelIndex].transform.rotate.x, 0.01f);
                // モデルのスケール
                ImGui::DragFloat3("Model Scale", &model.models[modelIndex].transform.scale.x, 0.01f);
                // モデルのマテリアルカラー
                ImGui::DragFloat4("Model MaterialColor", &model.models[modelIndex].material.color.x, 1.0f, 0.0f, 255.0f);
                // モデルのテクスチャインデックス
                ImGui::InputInt("Model TextureIndex", &model.models[modelIndex].useTextureIndex);
            }

            ImGui::TreePop();
        }

        // 板
        if (ImGui::TreeNode("板")) {
            ImGui::DragFloat3("Plane Translate", &floor.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Plane Rotate", &floor.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Plane Scale", &floor.transform.scale.x, 0.01f);
            ImGui::DragFloat4("Plane MaterialColor", &floor.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Plane TextureIndex", &floor.useTextureIndex);
            if (ImGui::TreeNode("Plane uvTransform")) {
                ImGui::DragFloat2("Plane uvTransform Translate", &floor.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Plane uvTransform Rotate", &floor.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Plane uvTransform Scale", &floor.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::End();

        //==================================================
        // 描画処理
        //==================================================

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));
        // ブレンドモードを設定
        drawer->SetBlendMode(blendMode);
        // 平行光源を設定
        drawer->SetLight(&directionalLight);

        // 球体の描画
        drawer->DrawSet(&sphere);
        // モデルの描画
        for (auto &modelData : model.models) {
            drawer->DrawSet(&modelData);
        }
        // 板の描画
        drawer->DrawSet(&floor);
        
        drawer->PostDraw();
        myGameEngine->EndFrame();

        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}