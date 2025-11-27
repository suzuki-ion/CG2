#include "GameScene.h"
#include "Base/SceneManager.h"
#include "Base/Input.h"
#include "Common/Logs.h"
#include "Base/Texture.h"
#include "KashipanEngine.h"
#include "Base/WinApp.h"
#include "Base/Renderer.h"
#include "CameraController.h"
#include <random>

namespace KashipanEngine {

GameScene::GameScene() : SceneBase("Game") {
    groundTextureIndex_ = Texture::Load("Resources/uvChecker.png");
    playerTextureIndex_ = Texture::Load("Resources/testPlayer.png");
    particleTextureIndex_ = Texture::Load("Resources/uvChecker.png");

    // 地面生成 (Y=0 に配置)
    ground_ = std::make_unique<Plane>();
    {
        auto st = ground_->GetStatePtr();
        st.transform->translate = {0.0f, 0.0f, 0.0f};
        st.transform->scale = {32.0f, 1.0f, 32.0f};
        *st.useTextureIndex = groundTextureIndex_;
        st.material->lightingType = 0;
    }
}

void GameScene::Initialize() {
    if (isInitialized_) { return; }
    Log("GameScene Initialize.");
    // カメラ生成 (X角度 45度)
    if (!camera_) {
        auto *winApp = engine_->GetWinApp();
        int width = winApp ? winApp->GetClientWidth() : 1280;
        int height = winApp ? winApp->GetClientHeight() : 720;
        camera_ = std::make_unique<Camera>(Vector3(0.0f, 16.0f, -16.0f), Vector3(0.785398163f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)); // 45deg = PI/4
        CameraPerspective persp{};
        persp.fovY = 0.5f;
        persp.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        persp.nearClip = 0.1f;
        persp.farClip = 200.0f;
        camera_->SetCameraPerspective(persp);
        CameraViewport vp{};
        vp.left = 0.0f; vp.top = 0.0f; vp.width = static_cast<float>(width); vp.height = static_cast<float>(height); vp.minDepth = 0.0f; vp.maxDepth = 1.0f;
        camera_->SetCameraViewport(vp);
        engine_->GetRenderer()->SetCamera(camera_.get());
    }

    // CameraController 生成・設定
    cameraController_ = std::make_unique<CameraController>(camera_.get());

    // プレイヤー生成
    player_ = std::make_unique<Player>(camera_->GetRotate(), playerTextureIndex_);

    // パーティクル生成
    particleGroup_ = ParticleManager::CreateParticleGroup(
        "GameParticles", engine_->GetDxCommon(), 256, static_cast<uint32_t>(particleTextureIndex_));
    particleGroup_->SetSpawnPosition({0.0f, 0.0f, 0.0f});

    // ランダム配置設定
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> distX(-16.0f, 16.0f);
    std::uniform_real_distribution<float> distY(1.0f, 12.0f);
    std::uniform_real_distribution<float> distZ(-16.0f, 16.0f);
    std::uniform_real_distribution<float> distScale(0.5f, 2.0f);

    for (int i = 0; i < 64; ++i) {
        if (auto *p = particleGroup_->SpawnParticle()) {
            p->transform.translate = {distX(rng), distY(rng), distZ(rng)};
            float s = distScale(rng);
            p->transform.scale = {s, s, s};
            p->transform.rotate = camera_->GetRotate();
        }
    }

    isInitialized_ = true;
}

void GameScene::Finalize() {
    if (!isInitialized_) { return; }
    Log("GameScene Finalize.");
    cameraController_.reset();
    camera_.reset();
    player_.reset();
    isInitialized_ = false;
}

void GameScene::Update() {
    if (Input::IsKeyTrigger(DIK_E)) {
        SceneManager::SetActiveScene("Clear");
    }

    // プレイヤー更新 (WASD移動)
    if (player_) { player_->Update(); }

    // カメラ追従: targetPosition をプレイヤー位置に毎フレーム設定
    if (cameraController_ && player_) {
        cameraController_->SetTargetPosition(player_->GetPosition());
        cameraController_->Update();
    }

    // パーティクル簡易浮遊ロジック (Y維持しないよう修正: Y=0固定)
    if (particleGroup_) {
        const auto &list = particleGroup_->GetParticles();
        for (size_t i = 0; i < list.size(); ++i) {
            auto &p = list[i];
            if (!p->isAlive || !p->isActive) { continue; }
        }
    }
}

void GameScene::Draw() {
    if (ground_) { ground_->Draw(); }
    if (player_) { player_->Draw(); }
    if (particleGroup_) { particleGroup_->Draw(); }
}

} // namespace KashipanEngine
