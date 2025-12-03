#include <KashipanEngine.h>
#include <memory>
#include <fstream>
#include <cmath>
#ifdef USE_IMGUI
#include "2d/ImGuiManager.h"
#endif

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Renderer.h"
#include "Base/Input.h"

#include "Math/Camera.h"

#include "3d/DirectionalLight.h"
#include "Objects.h"

#include "Common/Random.h"

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("LE2A_09_スズキ_イオン", 1920, 1080, false);

    // WinAppクラスへのポインタ
    WinApp *winApp = myGameEngine->GetWinApp();
    winApp->SetSizeChangeMode(SizeChangeMode::kNormal);

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    // デバッグカメラ切り替え用フラグ
    bool isDebugCameraActive = false;

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight{};
    directionalLight.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    directionalLight.direction = Vector3(0.5f, -0.75f, -0.5f).Normalize();
    directionalLight.intensity = 16.0f;

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>();
    // カメラをレンダラーにセット
    myGameEngine->GetRenderer()->SetCamera(camera.get());
    camera->SetTranslate({ 0.0f, 0.0f, -32.0f });
    camera->SetCoordinateSystem(Camera::CoordinateSystem::kSpherical);

    //==================================================
    // 球体モデル
    //==================================================

    Sphere sphere(16);
    *sphere.GetStatePtr().normalType = NormalType::kNormalTypeFace;

    //==================================================
    // パーティクル
    //==================================================

    const int kInstanceCount = static_cast<int>(pow(2, 21));
    ParticleGroup *particleGroup = ParticleManager::CreateParticleGroup(
        "TestParticles",
        myGameEngine->GetDxCommon(),
        kInstanceCount, // 最大インスタンス数
        0 // 使用テクスチャ
    );
    particleGroup->SetUseCompute(true);

    for (int i = 0; i < kInstanceCount; ++i) {
        if (auto *p = particleGroup->SpawnParticle()) {
            float random = GetRandomFloat(0.1f, 0.5f);
            p->transform.translate = { 0.0f, 0.0f, 0.0f };
            p->transform.scale = { random, random, random };
            p->transform.rotate = { 0.0f, 0.0f, 0.0f };
        }
    }

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        //==================================================
        // 更新処理
        //==================================================

        // 平行光源をレンダラーにセット
        myGameEngine->GetRenderer()->SetLight(&directionalLight);
#ifdef USE_IMGUI
        ImGuiManager::Begin("KashipanEngine");
        // FPSの表示
        ImGui::Text("FPS: %d", myGameEngine->GetFPS());
        // デルタタイムの表示
        ImGui::Text("DeltaTime: %f", myGameEngine->GetDeltaTime());
        ImGui::InputInt("フレームレート", &frameRate);
        if (ImGui::Button("フレームレートを設定")) {
            myGameEngine->SetFrameRate(frameRate);
        }
        // デバッグカメラの有効化
        if (ImGui::Checkbox("デバッグカメラの有効化", &isDebugCameraActive)) {
            myGameEngine->GetRenderer()->ToggleDebugCamera();
        }
        ImGui::End();

        // パーティクル
        ImGui::Begin("パーティクル");
        ImGui::Text("アクティブパーティクル数: %d", particleGroup->GetInstanceCount());
        auto *particleCB = particleGroup->GetGpuParticleCB();

        ImGui::InputFloat3("Spawn Position", &particleCB->spawnPos.x);
        ImGui::Separator();
        ImGui::Text("Noise / Motion Parameters");
        ImGui::SliderFloat("Noise Scale", &particleCB->noiseScale, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise Amplitude", &particleCB->noiseAmplitudeBase, 0.0f, 10.0f);
        ImGui::SliderFloat("Time Speed X", &particleCB->timeSpeedX, 0.0f, 5.0f);
        ImGui::SliderFloat("Time Speed Y", &particleCB->timeSpeedY, 0.0f, 5.0f);
        ImGui::SliderFloat("Time Speed Z", &particleCB->timeSpeedZ, 0.0f, 5.0f);
        ImGui::SliderFloat("Curl Strength", &particleCB->curlStrength, 0.0f, 50.0f);
        ImGui::SliderFloat("Damping", &particleCB->damping, 0.0f, 1.0f);
        ImGui::SliderFloat("Epsilon", &particleCB->eps, 0.00001f, 0.1f);
        ImGui::Separator();
        ImGui::Text("Bounding Box (half-extent)");
        ImGui::InputFloat3("Box Half Extent", &particleCB->boxHalfExtent.x);

        ImGui::End();
#else
        static_cast<void>(isDebugCameraActive);
#endif
        // カメラを常に反時計回りに回転
        auto sphericalCoordinateSystem = camera->GetSphericalCoordinateSystem();
        sphericalCoordinateSystem.theta += Engine::GetDeltaTime() * 0.25f;
        sphericalCoordinateSystem.phi = cos(sphericalCoordinateSystem.theta) * 0.5f + (std::numbers::pi_v<float> * 0.5f);
        camera->SetSphericalCoordinateSystem(sphericalCoordinateSystem);

        //==================================================
        // 描画処理
        //==================================================

        sphere.Draw();
        particleGroup->Draw();

        myGameEngine->EndFrame();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}