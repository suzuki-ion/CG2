#include "MainScene.h"
#include "KashipanEngine.h"
#include "Base/Renderer.h"
#include "Base/Input.h"
#include "Common/Random.h"
#include <cmath>

#ifdef USE_IMGUI
#include "2d/ImGuiManager.h"
#include <imgui.h>
#endif

using namespace KashipanEngine;

MainScene::MainScene()
    : SceneBase("MainScene"),
      sphere_(kDefaultSubdivision) {
}

void MainScene::Initialize() {
    // 平行光源
    directionalLight_.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    directionalLight_.direction = Vector3(0.5f, -0.75f, -0.5f).Normalize();
    directionalLight_.intensity = 16.0f;

    // カメラ
    camera_ = std::make_unique<Camera>();
    engine_->GetRenderer()->SetCamera(camera_.get());
    camera_->SetTranslate({ 0.0f, 0.0f, -32.0f });
    camera_->SetCoordinateSystem(Camera::CoordinateSystem::kSpherical);

    // 球体
    auto state = sphere_.GetStatePtr();
    if (state.normalType) {
        *state.normalType = KashipanEngine::kNormalTypeFace;
    }

    // パーティクル
    const int kInstanceCount = static_cast<int>(std::pow(2, 21));
    particleGroup_ = ParticleManager::CreateParticleGroup(
        "TestParticles",
        engine_->GetDxCommon(),
        kInstanceCount,
        0
    );
    particleGroup_->SetUseCompute(true);

    for (int i = 0; i < kInstanceCount; ++i) {
        if (auto *p = particleGroup_->SpawnParticle()) {
            float random = GetRandomFloat(0.1f, 0.5f);
            p->transform.translate = { 0.0f, 0.0f, 0.0f };
            p->transform.scale = { random, random, random };
            p->transform.rotate = { 0.0f, 0.0f, 0.0f };
        }
    }

    isInitialized_ = true;
}

void MainScene::Finalize() {
    ParticleManager::ClearAllParticleGroups();
    isInitialized_ = false;
}

void MainScene::Update() {
    // 平行光源をレンダラーにセット
    engine_->GetRenderer()->SetLight(&directionalLight_);

#ifdef USE_IMGUI
    ImGuiManager::Begin("KashipanEngine");
    ImGui::Text("FPS: %d", engine_->GetFPS());
    ImGui::Text("DeltaTime: %f", engine_->GetDeltaTime());
    ImGui::InputInt("フレームレート", &frameRate_);
    if (ImGui::Button("フレームレートを設定")) {
        engine_->SetFrameRate(frameRate_);
    }
    if (ImGui::Checkbox("デバッグカメラの有効化", &isDebugCameraActive_)) {
        engine_->GetRenderer()->ToggleDebugCamera();
    }
    ImGui::End();

    ImGui::Begin("パーティクル");
    ImGui::Text("アクティブパーティクル数: %d", particleGroup_->GetInstanceCount());
    auto *particleCB = particleGroup_->GetGpuParticleCB();

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
    static_cast<void>(isDebugCameraActive_);
#endif

    // カメラを常に反時計回りに回転
    auto sphericalCoordinateSystem = camera_->GetSphericalCoordinateSystem();
    sphericalCoordinateSystem.theta += Engine::GetDeltaTime() * 0.25f;
    sphericalCoordinateSystem.phi = std::cos(sphericalCoordinateSystem.theta) * 0.5f + (std::numbers::pi_v<float> * 0.5f);
    camera_->SetSphericalCoordinateSystem(sphericalCoordinateSystem);
}

void MainScene::Draw() {
    sphere_.Draw();
    if (particleGroup_) {
        particleGroup_->Draw();
    }
}
