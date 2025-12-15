#include "TestScene.h"
#include "KashipanEngine.h"
#include "Base/Renderer.h"
#include "Base/Input.h"

#ifdef USE_IMGUI
#include "2d/ImGuiManager.h"
#include <imgui.h>
#endif

using namespace KashipanEngine;

TestScene::TestScene()
    : SceneBase("TestScene") {}

void TestScene::Initialize() {
    // 平行光源
    directionalLight_.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    directionalLight_.direction = Vector3(0.5f, -0.75f, -0.5f).Normalize();
    directionalLight_.intensity = 16.0f;

    // カメラ
    camera_ = std::make_unique<Camera>();
    engine_->GetRenderer()->SetCamera(camera_.get());
    camera_->SetTranslate({ 0.0f, 0.0f, -32.0f });
    camera_->SetCoordinateSystem(Camera::CoordinateSystem::kSpherical);

    // テキスト
    titleText_ = std::make_unique<Text>(128);
    titleText_->SetFont("Resources/Font/test.fnt");
    titleText_->SetText(u8"TestScene：スペースキーでMainSceneに遷移");

    isInitialized_ = true;
}

void TestScene::Finalize() {
    ParticleManager::ClearAllParticleGroups();
    isInitialized_ = false;
}

void TestScene::Update() {
    // 平行光源をレンダラーにセット
    engine_->GetRenderer()->SetLight(&directionalLight_);

#ifdef USE_IMGUI
    ImGuiManager::Begin("KashipanEngine");
    ImGui::Text("FPS: %d", engine_->GetFPS());
    ImGui::Text("DeltaTime: %f", engine_->GetDeltaTime());
    ImGui::InputInt("フレームレート", &frameRate_);
    ImGui::End();
#else
    static_cast<void>(isDebugCameraActive_);
#endif

    if (Input::IsKeyTrigger(DIK_SPACE)) {
        SceneManager::SetActiveScene("MainScene");
    }
}

void TestScene::Draw() {
    titleText_->Draw();
}
