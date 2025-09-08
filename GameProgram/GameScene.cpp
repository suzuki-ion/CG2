#include <Base/WinApp.h>
#include <Base/DirectXCommon.h>
#include <Base/Renderer.h>
#include <Base/Texture.h>
#include <Base/Sound.h>
#include <Base/SceneManager.h>
#include <Common/Random.h>
#include "GameScene.h"
#include "GameScene/Enemies/EnemyNormal.h"

using namespace KashipanEngine;

GameScene::GameScene() : SceneBase("GameScene") {
    engine_->GetDxCommon()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    //==================================================
    // カメラ生成
    //==================================================

    camera2D_ = std::make_unique<Camera>();
    camera2D_->SetDimension(Camera::Dimension::k2D);

    CameraOrthographic cameraOrthographic;
    cameraOrthographic.left = 0.0f;
    cameraOrthographic.top = 1080.0f;
    cameraOrthographic.right = 1920.0f;
    cameraOrthographic.bottom = 0.0f;
    cameraOrthographic.nearClip = 0.0f;
    cameraOrthographic.farClip = 1.0f;
    camera2D_->SetCameraOrthographic(cameraOrthographic);
    
    CameraViewport cameraViewport2D;
    cameraViewport2D.left = 0.0f;
    cameraViewport2D.top = 0.0f;
    cameraViewport2D.width = 1920.0f;
    cameraViewport2D.height = 1080.0f;
    cameraViewport2D.minDepth = 0.0f;
    cameraViewport2D.maxDepth = 1.0f;
    camera2D_->SetCameraViewport(cameraViewport2D);

    //==================================================
    // スプライト生成
    //==================================================


    Sprite sprite("Resources/testPlayer.png");
    sprite.SetPipelineName("Object2d.BlendNormal");
    sprite.SetIsUseCamera(true);

    //==================================================
    // キーコンフィグ設定
    //==================================================

    // プレイヤーのキーコンフィグ設定
    playerKeyConfig_.LoadFromJson("Resources/KeyConfig/PlayerKeyConfig.json");
    // UI操作用のキーコンフィグ設定
    uiKeyConfig_.LoadFromJson("Resources/KeyConfig/UISelectKeyConfig.json");

    //==================================================
    // 音声読み込み
    //==================================================


    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    sceneStartFadeIn_ = std::make_unique<FadeIn>(1.0f, Vector4(0.0f), 1920.0f, 1080.0f);
    sceneEndFadeOut_ = std::make_unique<FadeOut>(1.0f, Vector4(0.0f), 1920.0f, 1080.0f);
};

void GameScene::Initialize() {
    engine_->GetRenderer()->SetCamera(camera2D_.get());

    //==================================================
    // オブジェクト設定
    //==================================================

    //==================================================
    // インスタンス生成
    //==================================================

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    nextSceneName_ = "";
    sceneEndFadeOut_->Stop();
    sceneStartFadeIn_->Play();
    
    isInitialized_ = true;
}

void GameScene::Finalize() {
    bullets_.clear();
    deathParticles_.clear();
    enemies_.clear();
    isInitialized_ = false;
}

void GameScene::Update() {
#ifdef _DEBUG

#endif
    UpdateInputDevice();
    UpdateSceneChangeAnimation();
    UpdatePauseMenu();
    // ポーズメニューがアクティブなら、以降の更新処理を行わない
    if (pauseMenu_->IsActive()) {
        return;
    }
    UpdatePlayer();
    UpdateEnemy();
    UpdateBullet();
    UpdateCollision();
    UpdateDeathParticles();
    UpdatePlayerHealthIcons();
}

void GameScene::Draw() {
    player_->Draw();
    for (const auto& enemy : enemies_) {
        enemy->Draw();
    }
    for (const auto& bullet : bullets_) {
        bullet->Draw();
    }
    for (const auto &particle : deathParticles_) {
        particle->Draw();
    }
    playerHealthIcons_->Draw();
    pauseMenu_->Draw();
    sceneStartFadeIn_->Draw();
    sceneEndFadeOut_->Draw();
}

void GameScene::UpdateInputDevice() {
    InputDeviceType nowInputDevice = Input::GetCurrentInputDeviceType();
    if (nowInputDevice != InputDeviceType::None) {
        currentInputDevice_ = nowInputDevice;
    }
}

void GameScene::UpdateSceneChangeAnimation() {
    sceneStartFadeIn_->Update();
    sceneEndFadeOut_->Update();
}

void GameScene::UpdatePauseMenu() {
    if (IsPauseCondition()) {
        if (std::get<bool>(uiKeyConfig_.GetInputValue("TogglePause"))) {
            if (!pauseMenu_->IsActive()) {
                pauseMenu_->Open();
            } else {
                pauseMenu_->Close();
            }
        }
    }

    if (!pauseMenu_->IsActive()) {
        return;
    }

    if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectUp"))) {
        pauseMenu_->SelectUp();
    }
    if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectDown"))) {
        pauseMenu_->SelectDown();
    }
    if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectConfirm"))) {
        pauseMenu_->Confirm();
        PauseMenuButton button = pauseMenu_->GetSelectedButton();
    }
    pauseMenu_->Update();
}

void GameScene::UpdatePlayer() {
    if (IsPlayerMoveCondition()) {
        Vector2 moveInput = Vector2(0.0f, 0.0f);
        moveInput.x = std::get<float>(playerKeyConfig_.GetInputValue("MoveHorizontal"));
        moveInput.y = std::get<float>(playerKeyConfig_.GetInputValue("MoveVertical"));
        player_->SetMoveDirection(moveInput);
    }
    player_->Update();
}

void GameScene::UpdateEnemy() {
    for (const auto &enemy : enemies_) {
        enemy->Update();
    }
}

void GameScene::UpdateBullet() {
    for (const auto &bullet : bullets_) {
        bullet->Update();
    }
    // 一定範囲を超えた弾を削除
    bullets_.remove_if([this](const std::unique_ptr<BaseBullet> &bullet) {
        return
            (bullet->GetPosition().x < 0.0f || bullet->GetPosition().x > 1920.0f) ||
            (bullet->GetPosition().y < 0.0f || bullet->GetPosition().y > 1080.0f);
        });
}

void GameScene::UpdateCollision() {
    CheckAllCollisions();
    // 死んだ敵を削除
    enemies_.remove_if([this](const std::unique_ptr<BaseEntity> &enemy) {
        if (enemy->IsAlive()) {
            return false;
        }
        // 敵の死亡パーティクルを生成
        Vector3 enemyPos = enemy->GetWorldPosition();
        std::unique_ptr<DeathParticle> particle = std::make_unique<DeathParticle>(
            deathParticleModel_.get(), enemyPos);
        deathParticles_.emplace_back(std::move(particle));
        // 殺害SEを再生
        Sound::Play(seKillIndex_, 0.5f, 0.0f, false);
        return true;
        });

    // 当たった弾を削除
    bullets_.remove_if([](const std::unique_ptr<BaseBullet> &bullet) {
        return !bullet->IsAlive();
        });
}

void GameScene::UpdateDeathParticles() {
    for (const auto &particle : deathParticles_) {
        particle->Update();
    }
    // 寿命が尽きたパーティクルを削除
    deathParticles_.remove_if([](const std::unique_ptr<DeathParticle> &particle) {
        return !particle->IsAlive();
        });
}

void GameScene::UpdatePlayerHealthIcons() {
    StatusParameters param = player_->GetStatus();
    playerHealthIcons_->SetHealthMax(param.healthMax);
    playerHealthIcons_->SetHealthCurrent(param.healthCurrent);
    playerHealthIcons_->Update();
}

void GameScene::CheckAllCollisions() {
    collisionManager_->ClearColliders();
    if (player_->IsAlive()) {
        collisionManager_->AddCollider(player_.get());
    }
    for (const auto &enemy : enemies_) {
        collisionManager_->AddCollider(enemy.get());
    }
    for (const auto &bullet : bullets_) {
        collisionManager_->AddCollider(bullet.get());
    }
    collisionManager_->Update();
}

bool GameScene::IsPauseCondition() {
    return player_->IsAlive() &&
        !sceneStartFadeIn_->IsPlaying() &&
        !sceneEndFadeOut_->IsPlaying();
}

bool GameScene::IsPlayerMoveCondition() {
    return !pauseMenu_->IsActive() &&
        !sceneStartFadeIn_->IsPlaying() &&
        !sceneEndFadeOut_->IsPlaying();
}
