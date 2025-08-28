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

    // カメラの生成
    camera_ = std::make_unique<Camera>();
    camera_->SetTranslate(Vector3(0.0f, 16.0f, 0.0f));

    CameraPerspective cameraPerspective;
    cameraPerspective.fovY = 1.0f;
    float width = static_cast<float>(engine_->GetWinApp()->GetClientWidth());
    float height = static_cast<float>(engine_->GetWinApp()->GetClientHeight());
    cameraPerspective.aspectRatio = width / height;
    cameraPerspective.nearClip = 0.1f;
    cameraPerspective.farClip = 128.0f;

    CameraViewport cameraViewport;
    cameraViewport.left = 0.0f;
    cameraViewport.top = 0.0f;
    cameraViewport.width = width;
    cameraViewport.height = height;

    camera_->SetCameraPerspective(cameraPerspective);
    camera_->SetCameraViewport(cameraViewport);

    //==================================================
    // モデル生成
    //==================================================

    // プレイヤーのモデル生成
    playerModel_ = std::make_unique<Model>("Resources/Player", "player.obj");
    auto playerState = playerModel_->GetStatePtr();
    playerState.material->lightingType = 0;
    playerState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    // 敵のモデル生成
    enemyModel_ = std::make_unique<Model>("Resources/Enemy", "enemy.obj");
    auto enemyState = enemyModel_->GetStatePtr();
    enemyState.material->lightingType = 0;
    enemyState.material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // プレイヤーの弾のモデル生成
    playerBulletModel_ = std::make_unique<Model>("Resources/Bullet", "bullet.obj");
    auto bulletState = playerBulletModel_->GetStatePtr();
    bulletState.material->lightingType = 0;
    bulletState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    // 敵の弾のモデル生成
    enemyBulletModel_ = std::make_unique<Model>("Resources/Bullet", "bullet.obj");
    auto enemyBulletState = enemyBulletModel_->GetStatePtr();
    enemyBulletState.material->lightingType = 0;
    enemyBulletState.material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // 背景パーティクル用モデル生成
    backgroundParticleModel_ = std::make_unique<Model>("Resources/Block", "block.obj");
    backgroundParticleModel_->SetPipelineName("Object3d.DoubleSidedCulling.BlendNormal");
    auto particleBlockGreenState = backgroundParticleModel_->GetStatePtr();
    particleBlockGreenState.material->lightingType = 0;
    particleBlockGreenState.material->color = Vector4(0.0f, 0.5f, 0.0f, 1.0f);

    // 死亡パーティクル用モデル生成
    deathParticleModel_ = std::make_unique<Model>("Resources/Block", "block.obj");
    deathParticleModel_->SetPipelineName("Object3d.DoubleSidedCulling.BlendNormal");
    auto particleBlockRedState = deathParticleModel_->GetStatePtr();
    particleBlockRedState.material->lightingType = 0;
    particleBlockRedState.material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // グリッドライン(床)の板ポリ生成
    gridLineFloor_ = std::make_unique<Plane>();
    auto gridLineState = gridLineFloor_->GetStatePtr();
    *gridLineState.useTextureIndex = Texture::Load("Resources/grid.png");
    gridLineState.mesh->vertexBufferMap[0].position = Vector4(-10000.0f, 0.0f, -10000.0f, 1.0f);
    gridLineState.mesh->vertexBufferMap[1].position = Vector4(-10000.0f, 0.0f, 10000.0f, 1.0f);
    gridLineState.mesh->vertexBufferMap[2].position = Vector4(10000.0f, 0.0f, -10000.0f, 1.0f);
    gridLineState.mesh->vertexBufferMap[3].position = Vector4(10000.0f, 0.0f, 10000.0f, 1.0f);
    gridLineState.uvTransform->scale = Vector2(500.0f, 500.0f);
    gridLineState.material->lightingType = 0;
    gridLineState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    // グリッドライン(左壁)の板ポリ生成
    gridLineLeftWall_ = std::make_unique<Plane>();
    auto gridLineLeftWallState = gridLineLeftWall_->GetStatePtr();
    *gridLineLeftWallState.useTextureIndex = Texture::Load("Resources/gridVertical.png");
    gridLineLeftWallState.mesh->vertexBufferMap[0].position = Vector4(-32.0f, 0.0f, -10000.0f, 1.0f);
    gridLineLeftWallState.mesh->vertexBufferMap[1].position = Vector4(-32.0f, 20000.0f, -10000.0f, 1.0f);
    gridLineLeftWallState.mesh->vertexBufferMap[2].position = Vector4(-32.0f, 0.0f, 10000.0f, 1.0f);
    gridLineLeftWallState.mesh->vertexBufferMap[3].position = Vector4(-32.0f, 20000.0f, 10000.0f, 1.0f);
    gridLineLeftWallState.uvTransform->scale = Vector2(1000.0f, 1000.0f);
    gridLineLeftWallState.material->lightingType = 0;
    gridLineLeftWallState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    // グリッドライン(右壁)の板ポリ生成
    gridLineRightWall_ = std::make_unique<Plane>();
    auto gridLineRightWallState = gridLineRightWall_->GetStatePtr();
    *gridLineRightWallState.useTextureIndex = Texture::Load("Resources/gridVertical.png");
    gridLineRightWallState.mesh->vertexBufferMap[0].position = Vector4(32.0f, 0.0f, 10000.0f, 1.0f);
    gridLineRightWallState.mesh->vertexBufferMap[1].position = Vector4(32.0f, 20000.0f, 10000.0f, 1.0f);
    gridLineRightWallState.mesh->vertexBufferMap[2].position = Vector4(32.0f, 0.0f, -10000.0f, 1.0f);
    gridLineRightWallState.mesh->vertexBufferMap[3].position = Vector4(32.0f, 20000.0f, -10000.0f, 1.0f);
    gridLineRightWallState.uvTransform->scale = Vector2(1000.0f, 1000.0f);
    gridLineRightWallState.material->lightingType = 0;
    gridLineRightWallState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    // ダメージ量表示用テキストの生成
    damageAmountText_ = std::make_unique<Text>(1);
    damageAmountText_->SetFont("Resources/Font/jfdot-k14_64.fnt");

    // リザルト時タイトルに戻る方法表示用テキスト
    returnToTitlePromptText_ = std::make_unique<Text>(32);
    returnToTitlePromptText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    returnToTitlePromptText_->SetText(u8"Escapeでタイトルにもどる");
    returnToTitlePromptText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    auto returnToTitlePromptTextState = returnToTitlePromptText_->GetStatePtr();
    returnToTitlePromptTextState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    returnToTitlePromptTextState.transform->translate = Vector3(1920.0f / 2.0f, 1080.0f / 5.0f * 4.0f, 0.0f);

    // 弾切り替え方法表示用テキストの生成
    bulletChangePromptText_ = std::make_unique<Text>(32);
    bulletChangePromptText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    bulletChangePromptText_->SetText(u8"ChangeBullet: Q/E");
    bulletChangePromptText_->SetTextAlign(TextAlignX::Left, TextAlignY::Bottom);
    auto bulletChangePromptTextState = bulletChangePromptText_->GetStatePtr();
    bulletChangePromptTextState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    bulletChangePromptTextState.transform->translate = Vector3(16.0f, 1080.0f - 96.0f, 0.0f);
    bulletChangePromptTextState.transform->scale = Vector3(0.5f, 0.5f, 1.0f);

    // プレイヤーのレベル表示用テキストの生成
    playerLevelText_ = std::make_unique<Text>(16);
    playerLevelText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    playerLevelText_->SetTextAlign(TextAlignX::Left, TextAlignY::Top);
    auto playerLevelTextState = playerLevelText_->GetStatePtr();
    playerLevelTextState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    playerLevelTextState.transform->translate = Vector3(16.0f, 152.0f, 0.0f);
    playerLevelTextState.transform->scale = Vector3(0.5f, 0.5f, 1.0f);

    // プレイヤーのキーコンフィグ設定
    playerKeyConfig_.LoadFromJson("Resources/KeyConfig/PlayerKeyConfig.json");
    // UI操作用のキーコンフィグ設定
    uiKeyConfig_.LoadFromJson("Resources/KeyConfig/UISelectKeyConfig.json");

    //==================================================
    // 音声読み込み
    //==================================================

    // BGM(通常ウェーブ)の読み込み
    bgmNormalWaveIndex_ = Sound::Load("Resources/BGM/gameNormalWave.mp3");
    // BGM(ラストウェーブ)の読み込み
    bgmLastWaveIndex_ = Sound::Load("Resources/BGM/gameLastWave.mp3");
    // BGM(ウェーブクリア)の読み込み
    bgmClearWaveIndex_ = Sound::Load("Resources/BGM/gameClearWave.mp3");

    // SE(レベルアップ)の読み込み
    seLevelUpIndex_ = Sound::Load("Resources/SE/levelUp.mp3");
    // SE(弾発射)の読み込み
    seShootIndex_ = Sound::Load("Resources/SE/bulletShoot.mp3");
    // SE(ダメージ)の読み込み
    seDamageIndex_ = Sound::Load("Resources/SE/damage.mp3");
    // SE(死亡)の読み込み
    seDeathIndex_ = Sound::Load("Resources/SE/death.mp3");
    // SE(通常ウェーブ開始)の読み込み
    seNormalWaveStartIndex_ = Sound::Load("Resources/SE/normalWaveStart.mp3");
    // SE(ラストウェーブ開始)の読み込み
    seLastWaveStartIndex_ = Sound::Load("Resources/SE/lastWaveStart.mp3");

    // SE(UI選択)の読み込み
    seUISelectIndex_ = Sound::Load("Resources/SE/uiSelect.mp3");
    // SE(UI決定)の読み込み
    seUIConfirmIndex_ = Sound::Load("Resources/SE/uiConfirm.mp3");
    // SE(UIキャンセル)の読み込み
    seUICancelIndex_ = Sound::Load("Resources/SE/uiCancel.mp3");
    // SE(リザルト時の決定)の読み込み
    seResultConfirmIndex_ = Sound::Load("Resources/SE/titleConfirm.mp3");

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    openRectToCenterY_ = std::make_unique<OpenRectToCenterY>(1.0f, 1920.0f, 1080.0f);
    closeRectToCenterY_ = std::make_unique<CloseRectToCenterY>(1.0f, 1920.0f, 1080.0f);
    fadeOut_ = std::make_unique<FadeOut>(2.0f, Vector4(0.0f), 1920.0f, 1080.0f);
};

void GameScene::Initialize() {
    engine_->GetRenderer()->SetCamera(camera_.get());

    //==================================================
    // オブジェクト設定
    //==================================================

    auto particleBlockGreenState = backgroundParticleModel_->GetStatePtr();
    particleBlockGreenState.material->color = Vector4(0.0f, 0.5f, 0.0f, 1.0f);

    auto gridLineState = gridLineFloor_->GetStatePtr();
    gridLineState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    auto gridLineLeftWallState = gridLineLeftWall_->GetStatePtr();
    gridLineLeftWallState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    auto gridLineRightWallState = gridLineRightWall_->GetStatePtr();
    gridLineRightWallState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    resultParameters_ = ResultParameters();
    returnToTitlePromptText_->Hide();

    //==================================================
    // インスタンス生成
    //==================================================

    // プレイヤーの生成
    player_ = std::make_unique<Player>(playerModel_.get(), playerBulletModel_.get(), camera_.get());
    player_->SetShootFunction([this](std::unique_ptr<BaseBullet> bullet) {
        this->AddBullet(std::move(bullet));
    });
    player_->SetShowDamageFunction([this](float damage, const KashipanEngine::Vector3 &position) {
        AddDamageAmountDisplay(damage, position);
    });
    player_->SetResultAddDamageFunction([this](float damage) {
        AddDamageTaken(damage);
    });

    // プレイヤーの体力ゲージ生成
    playerHealthBar_ = std::make_unique<PlayerHealthBar>();
    playerHealthBar_->SetHealthMax(player_->GetStatus().healthMax);
    // プレイヤーの経験値ゲージ生成
    playerExpBar_ = std::make_unique<PlayerExpBar>();
    playerExpBar_->SetExpCurrent(player_->GetStatus().exp);
    playerExpBar_->SetExpMax(player_->GetStatus().expToNextLevel);

    // プレイヤーの使用している弾の種類のテキスト生成
    playerBulletTypeText_ = std::make_unique<PlayerBulletTypeText>(1920.0f, 1080.0f);
    playerBulletTypeText_->SetBulletType(player_->GetBulletType());
    // ポーズメニューの生成
    pauseMenu_ = std::make_unique<PauseMenu>(1920.0f, 1080.0f);
    // ウェーブ表示の生成
    waveDisplay_ = std::make_unique<WaveDisplay>();
    // レベルアップ表示の生成
    playerLevelUpText_ = std::make_unique<PlayerLevelUpText>(camera_.get());
    // 発射クールダウンゲージ
    playerShootCoolDownBar_ = std::make_unique<PlayerShootCoolDownBar>(camera_.get());

    // 背景用のブロックパーティクル生成
    for (int i = 0; i < kBackgroundParticleBlockCount_; i++) {
        // パーティクルの進行方向
        Vector3 dir = { 0.0f, 0.0f, -1.0f };
        // パーティクルの回転方向をランダムに設定
        Vector3 rot = {
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f),
            GetRandomFloat(-1.0f, 1.0f)
        };

        // パーティクルの生成位置をランダムに設定
        float leftOrRight = static_cast<float>(GetRandomInt(0, 1) * 2 - 1); // -1 or 1
        Vector3 position = {
            GetRandomFloat(28.0f, 36.0f) * leftOrRight,
            GetRandomFloat(-10.0f, 64.0f),
            GetRandomFloat(0.0f, 128.0f)
        };

        std::unique_ptr<ParticleBlock> particle = std::make_unique<ParticleBlock>(
            backgroundParticleModel_.get(), position, dir, rot, 16.0f, 2.0f);
        backgroundParticleBlocks_.emplace_back(std::move(particle));
    }
    // レーザーポインターの生成
    laserPointer_ = std::make_unique<LaserPointer>();
    laserPointer_->SetLength(128.0f);

    // 当たり判定管理の初期化
    collisionManager_ = std::make_unique<CollisionManager>();

    // ウェーブ管理の初期化
    waveManager_ = std::make_unique<WaveManager>(
        "Resources/Waves/WaveNormal.json",
        [this](std::unique_ptr<BaseEntity> enemy) { this->AddEnemy(std::move(enemy)); },
        enemyModel_.get(), enemyBulletModel_.get(), camera_.get());
    waveManager_->LoadFromJson("Resources/Waves/WaveNormal.json");
    // ウェーブ開始アニメーションの生成
    waveDrawAnimation_ = std::make_unique<WaveDrawAnimation>(1920.0f, 1080.0f);
    // リザルト表示アニメーションの生成
    resultDrawAnimation_ = std::make_unique<ResultDrawAnimation>(1920.0f, 1080.0f);

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    nextSceneName_ = "";
    fadeOut_->Stop();
    closeRectToCenterY_->Stop();
    openRectToCenterY_->Play();
    
    isInitialized_ = true;
}

void GameScene::Finalize() {
    bullets_.clear();
    backgroundParticleBlocks_.clear();
    deathParticles_.clear();
    enemies_.clear();
    Sound::Stop(bgmNormalWaveIndex_);
    Sound::Stop(bgmLastWaveIndex_);
    Sound::Stop(bgmClearWaveIndex_);
    isInitialized_ = false;
}

void GameScene::Update() {
#ifdef _DEBUG
    if (Input::IsKeyTrigger(DIK_F1)) {
        float currentHealth = playerHealthBar_->GetHealthCurrent();
        playerHealthBar_->SetHealthCurrent(currentHealth - 10.0f);
    }
    if (Input::IsKeyTrigger(DIK_F2)) {
        float currentHealth = playerHealthBar_->GetHealthCurrent();
        playerHealthBar_->SetHealthCurrent(currentHealth + 10.0f);
    }
    if (Input::IsKeyTrigger(DIK_F3)) {
        engine_->GetRenderer()->ToggleDebugCamera();
    }
    if (Input::IsKeyDown(DIK_F4)) {
        float targetFovY = 0.5f;
        auto cameraPerspective = camera_->GetCameraPerspective();
        cameraPerspective.fovY = std::lerp(cameraPerspective.fovY, targetFovY, 0.1f);
        camera_->SetCameraPerspective(cameraPerspective);
    } else {
        float targetFovY = 1.0f;
        auto cameraPerspective = camera_->GetCameraPerspective();
        cameraPerspective.fovY = std::lerp(cameraPerspective.fovY, targetFovY, 0.1f);
        camera_->SetCameraPerspective(cameraPerspective);
    }
    if (Input::IsKeyTrigger(DIK_F5)) {
        Vector3 pos = {
            GetRandomFloat(-28.0f, 28.0f),
            GetRandomFloat(0.0f, 20.0f),
            128.0f
        };
        std::unique_ptr<BaseEntity> enemy = std::make_unique<EnemyNormal>(
            pos, enemyModel_.get(), enemyBulletModel_.get(), camera_.get());
        enemies_.emplace_back(std::move(enemy));
    }
    if (Input::IsKeyTrigger(DIK_F6)) {
        waveManager_->StartWave();
    }
    if (Input::IsKeyTrigger(DIK_F7)) {
        player_->SetLevel(player_->GetStatus().level - 1);
        playerHealthBar_->SetHealthMax(player_->GetStatus().healthMax);
    }
    if (Input::IsKeyTrigger(DIK_F8)) {
        player_->SetLevel(player_->GetStatus().level + 1);
        playerHealthBar_->SetHealthMax(player_->GetStatus().healthMax);
    }
#endif

    //==================================================
    // 表示テキスト更新
    //==================================================

    // 入力デバイスの変化を検出して、表示テキストを変更
    InputDeviceType nowInputDevice = Input::GetCurrentInputDeviceType();
    if (nowInputDevice != InputDeviceType::None &&
        currentInputDevice_ != nowInputDevice) {
        switch (nowInputDevice) {
            case KashipanEngine::InputDeviceType::None:
                break;
            case KashipanEngine::InputDeviceType::Keyboard:
            case KashipanEngine::InputDeviceType::Mouse:
                bulletChangePromptText_->SetText(u8"ChangeBullet: Q/E");
                returnToTitlePromptText_->SetText(u8"Escapeでタイトルにもどる");
                break;
            case KashipanEngine::InputDeviceType::XBoxController:
                bulletChangePromptText_->SetText(u8"ChangeBullet: LB/RB");
                returnToTitlePromptText_->SetText(u8"Bでタイトルにもどる");
                break;
            default:
                break;
        }
        currentInputDevice_ = nowInputDevice;
    }

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    openRectToCenterY_->Update();
    closeRectToCenterY_->Update();
    fadeOut_->Update();
    if (openRectToCenterY_->IsFinishedTrigger()) {
        waveDrawAnimation_->SetWaveNumber(waveManager_->GetCurrentWaveIndex() + 1, false, false);
        waveDrawAnimation_->Play();
    }
    if (closeRectToCenterY_->IsFinished() && nextSceneName_ != "") {
        SceneManager::SetActiveScene(nextSceneName_);
        return;
    }
    if (fadeOut_->IsFinished()) {
        SceneManager::SetActiveScene("GameOverScene");
        return;
    }

    //==================================================
    // ポーズメニュー
    //==================================================

    if (player_->IsAlive() &&
        !openRectToCenterY_->IsPlaying() &&
        !closeRectToCenterY_->IsPlaying() &&
        !fadeOut_->IsPlaying() &&
        !waveDrawAnimation_->IsClearWave()) {
        if (std::get<bool>(uiKeyConfig_.GetInputValue("TogglePause"))) {
            if (!pauseMenu_->IsActive()) {
                pauseMenu_->Open();
                Sound::Play(seUICancelIndex_, 0.5f, 0.0f, false);
                Sound::SetVolume(bgmNormalWaveIndex_, 0.2f);
            } else {
                pauseMenu_->Close();
                Sound::Play(seUICancelIndex_, 0.5f, 0.0f, false);
                Sound::SetVolume(bgmNormalWaveIndex_, 0.5f);
            }
        }
        if (pauseMenu_->IsActive()) {
            if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectUp"))) {
                pauseMenu_->SelectUp();
                Sound::Play(seUISelectIndex_, 0.5f, 0.0f, false);
            }
            if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectDown"))) {
                pauseMenu_->SelectDown();
                Sound::Play(seUISelectIndex_, 0.5f, 0.0f, false);
            }
            if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectConfirm"))) {
                pauseMenu_->Confirm();
                PauseMenuButton button = pauseMenu_->GetSelectedButton();
                Sound::Play(seUIConfirmIndex_, 0.5f, 0.0f, false);
                switch (button) {
                    case PauseMenuButton::kContinue:
                        pauseMenu_->Close();
                        Sound::SetVolume(bgmNormalWaveIndex_, 0.5f);
                        break;
                    case PauseMenuButton::kRestart:
                        nextSceneName_ = "GameScene";
                        closeRectToCenterY_->Play();
                        break;
                    case PauseMenuButton::kBackTitle:
                        nextSceneName_ = "TitleScene";
                        closeRectToCenterY_->Play();
                        break;
                    case PauseMenuButton::kExit:
                        engine_->QuitGame();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    pauseMenu_->Update();
    // ポーズメニューがアクティブなら、以降の更新処理を行わない
    if (pauseMenu_->IsActive()) {
        return;
    }

    //==================================================
    // ウェーブ開始アニメーション
    //==================================================

    waveDrawAnimation_->Update();
    if (waveDrawAnimation_->IsAnimationStartTrigger()) {
        Sound::Play(seNormalWaveStartIndex_, 0.5f, 0.0f, false);
        if (waveDrawAnimation_->IsLastWave() && !waveDrawAnimation_->IsClearWave()) {
            Sound::Play(seLastWaveStartIndex_, 0.5f, 0.0f, false);
        }
    }
    if (waveDrawAnimation_->IsPlaying()) {
        float elapsedTime = waveDrawAnimation_->GetElapsedTime();
        float duration = waveDrawAnimation_->GetDuration();
        float noneTime = waveDrawAnimation_->GetNoneTime();
        
        if (waveDrawAnimation_->IsClearWave()) {
            if (elapsedTime < noneTime) {
                float progress = elapsedTime / noneTime;
                Sound::SetVolume(bgmLastWaveIndex_, 0.5f * (1.0f - progress));
            } else {
                Sound::Stop(bgmLastWaveIndex_);
                float progress = (elapsedTime - noneTime) / (duration - noneTime);

                auto particleBlockGreenState = backgroundParticleModel_->GetStatePtr();
                particleBlockGreenState.material->color = Vector4::Lerp(Vector4(0.5f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.5f, 0.0f, 1.0f), progress);
                auto gridLineState = gridLineFloor_->GetStatePtr();
                gridLineState.material->color = Vector4::Lerp(Vector4(0.125f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.125f, 0.0f, 1.0f), progress);
                auto gridLineLeftWallState = gridLineLeftWall_->GetStatePtr();
                gridLineLeftWallState.material->color = Vector4::Lerp(Vector4(0.125f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.125f, 0.0f, 1.0f), progress);
                auto gridLineRightWallState = gridLineRightWall_->GetStatePtr();
                gridLineRightWallState.material->color = Vector4::Lerp(Vector4(0.125f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.125f, 0.0f, 1.0f), progress);
            }
        } else if (waveDrawAnimation_->IsLastWave()) {
            if (elapsedTime < noneTime) {
                float progress = elapsedTime / noneTime;
                Sound::SetVolume(bgmNormalWaveIndex_, 0.5f * (1.0f - progress));
            } else {
                Sound::Stop(bgmNormalWaveIndex_);
                float progress = (elapsedTime - noneTime) / (duration - noneTime);

                auto particleBlockGreenState = backgroundParticleModel_->GetStatePtr();
                particleBlockGreenState.material->color = Vector4::Lerp(Vector4(0.0f, 0.5f, 0.0f, 1.0f), Vector4(0.5f, 0.0f, 0.0f, 1.0f), progress);
                auto gridLineState = gridLineFloor_->GetStatePtr();
                gridLineState.material->color = Vector4::Lerp(Vector4(0.0f, 0.125f, 0.0f, 1.0f), Vector4(0.125f, 0.0f, 0.0f, 1.0f), progress);
                auto gridLineLeftWallState = gridLineLeftWall_->GetStatePtr();
                gridLineLeftWallState.material->color = Vector4::Lerp(Vector4(0.0f, 0.125f, 0.0f, 1.0f), Vector4(0.125f, 0.0f, 0.0f, 1.0f), progress);
                auto gridLineRightWallState = gridLineRightWall_->GetStatePtr();
                gridLineRightWallState.material->color = Vector4::Lerp(Vector4(0.0f, 0.125f, 0.0f, 1.0f), Vector4(0.125f, 0.0f, 0.0f, 1.0f), progress);
            }
        }
    }
    if (waveDrawAnimation_->IsFinishedTrigger()) {
        waveManager_->StartWave();
        if (waveManager_->GetCurrentWaveIndex() == 0) {
            Sound::Play(bgmNormalWaveIndex_, 0.5f, 0.0f, true);
        } else if (waveDrawAnimation_->IsClearWave()) {
            resultDrawAnimation_->SetResultParameters(resultParameters_);
            resultDrawAnimation_->Play();
            Sound::Play(bgmClearWaveIndex_, 0.5f, 0.0f, true);
        } else if (waveDrawAnimation_->IsLastWave()) {
            Sound::Play(bgmLastWaveIndex_, 0.5f, 0.0f, true);
        }
    }

    //==================================================
    // リザルト表示アニメーション
    //==================================================

    resultDrawAnimation_->Update();
    if (resultDrawAnimation_->IsFinishedTrigger()) {
        returnToTitlePromptText_->Show();
    }
    if (resultDrawAnimation_->IsFinished()) {
        if (std::get<bool>(uiKeyConfig_.GetInputValue("ResultBackTitle"))) {
            nextSceneName_ = "TitleScene";
            closeRectToCenterY_->Play();
            Sound::Play(seResultConfirmIndex_, 0.5f, 0.0f, false);
        }
    }

    //==================================================
    // 床と壁の動き
    //==================================================

    auto gridLineFloorState = gridLineFloor_->GetStatePtr();
    gridLineFloorState.uvTransform->translate.y -= 0.5f * Engine::GetDeltaTime();
    if (gridLineFloorState.uvTransform->translate.y < -1.0f) {
        gridLineFloorState.uvTransform->translate.y += 1.0f;
    }
    auto gridLineLeftWallState = gridLineLeftWall_->GetStatePtr();
    gridLineLeftWallState.uvTransform->translate.x += 1.0f * Engine::GetDeltaTime();
    if (gridLineLeftWallState.uvTransform->translate.x < -1.0f) {
        gridLineLeftWallState.uvTransform->translate.x -= 1.0f;
    }
    auto gridLineRightWallState = gridLineRightWall_->GetStatePtr();
    gridLineRightWallState.uvTransform->translate.x -= 1.0f * Engine::GetDeltaTime();
    if (gridLineRightWallState.uvTransform->translate.x < -1.0f) {
        gridLineRightWallState.uvTransform->translate.x += 1.0f;
    }

    //==================================================
    // ウェーブ管理の更新
    //==================================================

    waveManager_->Update();
    waveDisplay_->SetWave(
        waveManager_->GetCurrentWaveIndex() + 1,
        waveManager_->GetMaxWaveIndex()
    );
    if (player_->IsAlive() && waveManager_->IsWaveCleared() && (enemies_.size() == 0)) {
        // 敵の弾を全て消す
        bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) {
            return bullet->GetCollisionAttribute() == kCollisionAttributeEnemy;
        });
        if (!waveDrawAnimation_->IsLastWave()) {
            waveManager_->NextWavePrepare();
            bool isLastWave = waveManager_->GetCurrentWaveIndex() + 1 == waveManager_->GetMaxWaveIndex();
            waveDrawAnimation_->SetWaveNumber(waveManager_->GetCurrentWaveIndex() + 1, isLastWave, false);
            waveDrawAnimation_->Play();
        } else if (!waveDrawAnimation_->IsClearWave()) {
            waveDrawAnimation_->SetWaveNumber(waveManager_->GetCurrentWaveIndex() + 1, true, true);
            waveDrawAnimation_->Play();
        }
    }

    //==================================================
    // プレイヤー
    //==================================================

    if (!openRectToCenterY_->IsPlaying() && !closeRectToCenterY_->IsPlaying() && !fadeOut_->IsPlaying()) {
        Vector2 moveInput = Vector2(0.0f, 0.0f);
        moveInput.x = std::get<float>(playerKeyConfig_.GetInputValue("MoveHorizontal"));
        moveInput.y = std::get<float>(playerKeyConfig_.GetInputValue("MoveVertical"));
        player_->SetMoveDirection(moveInput);
        std::list<BaseEntity *> enemiesList;
        for (const auto &enemy : enemies_) {
            enemiesList.push_back(enemy.get());
        }
        player_->SetTargetList(enemiesList);
        player_->Update();

        if (std::get<bool>(playerKeyConfig_.GetInputValue("BulletChangePrev"))) {
            player_->ChangeBulletPrev();
            Sound::Play(seUISelectIndex_, 0.5f, 0.0f, false);
        }
        if (std::get<bool>(playerKeyConfig_.GetInputValue("BulletChangeNext"))) {
            player_->ChangeBulletNext();
            Sound::Play(seUISelectIndex_, 0.5f, 0.0f, false);
        }
        playerBulletTypeText_->SetBulletType(player_->GetBulletType());
        if (std::get<bool>(playerKeyConfig_.GetInputValue("BulletFire"))) {
            if (player_->Shoot()) {
                playerShootCoolDownBar_->SetCoolDownMax(player_->GetShootIntervalTimer());
                Sound::Play(seShootIndex_, 0.5f, 0.0f, false);
            }
        }
    }
    // レーザーポインターの更新
    Vector3 pointerPos = player_->GetWorldPosition();
    pointerPos.z += 4.0f; // プレイヤーの少し前にレーザーポインターを表示
    laserPointer_->SetPosition(pointerPos);
    // 弾発射クールダウンゲージの更新
    playerShootCoolDownBar_->SetCoolDownCurrent(player_->GetShootIntervalTimer());
    playerShootCoolDownBar_->SetPosition(player_->GetWorldPosition());

    //==================================================
    // 敵
    //==================================================

    for (const auto& enemy : enemies_) {
        enemy->Update();
    }
    // 一定範囲を超えた敵を削除
    enemies_.remove_if([this](const std::unique_ptr<BaseEntity>& enemy) {
        return (enemy->GetWorldPosition().z < 0.0f) ||
            (enemy->GetWorldPosition().z > camera_->GetCameraPerspective().farClip);
    });

    //==================================================
    // 弾
    //==================================================

    for (const auto &bullet : bullets_) {
        bullet->Update();
    }
    // 一定範囲を超えた弾を削除
    bullets_.remove_if([this](const std::unique_ptr<BaseBullet>& bullet) {
        return (bullet->GetPosition().z < 0.0f) ||
            (bullet->GetPosition().z > camera_->GetCameraPerspective().farClip);
    });

    //==================================================
    // 当たり判定
    //==================================================

    CheckAllCollisions();
    // 当たったエンティティがいれば効果音を再生
    if (collisionManager_->GetCollisionCount() > 0) {
        Sound::Play(seDamageIndex_, 0.5f, 0.0f, false);
    }

    // プレイヤーが死んだらゲームオーバーへ
    if (!player_->IsAlive() && !fadeOut_->IsPlaying()) {
        playerShootCoolDownBar_->SetCoolDownCurrent(0.0f);
        Sound::Stop(bgmNormalWaveIndex_);
        fadeOut_->Play();
        // 死亡時のパーティクルを生成
        Vector3 playerPos = player_->GetWorldPosition();
        std::unique_ptr<DeathParticle> particle = std::make_unique<DeathParticle>(
            deathParticleModel_.get(), playerPos);
        deathParticles_.emplace_back(std::move(particle));
    }

    // 死んだ敵を削除
    enemies_.remove_if([this](const std::unique_ptr<BaseEntity>& enemy) {
        if (enemy->IsAlive()) {
            return false;
        }
        // 敵の死亡パーティクルを生成
        Vector3 enemyPos = enemy->GetWorldPosition();
        std::unique_ptr<DeathParticle> particle = std::make_unique<DeathParticle>(
            deathParticleModel_.get(), enemyPos);
        deathParticles_.emplace_back(std::move(particle));
        // 敵の死亡SEを再生
        Sound::Play(seDeathIndex_, 0.5f, 0.0f, false);
        return true;
    });

    // 当たった弾を削除
    bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) {
        return !bullet->IsAlive();
    });

    //==================================================
    // 背景用ブロックパーティクル
    //==================================================

    for (const auto& particle : backgroundParticleBlocks_) {
        particle->Update();
        // Z座標が0.0f以下になったら、再度ランダムな位置に配置
        if (particle->GetPosition().z < 0.0f) {
            float leftOrRight = static_cast<float>(GetRandomInt(0, 1) * 2 - 1); // -1 or 1
            Vector3 position = {
                GetRandomFloat(24.0f, 64.0f) * leftOrRight,
                GetRandomFloat(-10.0f, 64.0f),
                particle->GetPosition().z + 128.0f
            };
            particle->SetPosition(position);
        }
    }

    //==================================================
    // 死亡パーティクル
    //==================================================

    for (const auto& particle : deathParticles_) {
        particle->Update();
    }
    // 寿命が尽きたパーティクルを削除
    deathParticles_.remove_if([](const std::unique_ptr<DeathParticle>& particle) {
        return !particle->IsAlive();
    });

    //==================================================
    // ダメージ量表示用テキスト
    //==================================================

    for (const auto& text : damageAmountDisplays_) {
        text->Update();
    }
    // 寿命が尽きたテキストを削除
    damageAmountDisplays_.remove_if([](const std::unique_ptr<DamageAmountDisplay>& text) {
        return !text->IsActive();
    });

    //==================================================
    // レベルアップ表示
    //==================================================

    // プレイヤーがレベルアップしたら表示
    if (player_->IsAlive() && player_->IsLevelUp()) {
        playerLevelUpText_->Show();
        Sound::Play(seLevelUpIndex_, 0.5f, 0.0f, false);
    }
    playerLevelUpText_->SetPosition(player_->GetWorldPosition());
    playerLevelUpText_->Update();

    //==================================================
    // プレイヤーの体力ゲージ
    //==================================================
    
    StatusParameters param = player_->GetStatus();
    playerHealthBar_->SetHealthMax(param.healthMax);
    playerHealthBar_->SetHealthCurrent(param.healthCurrent);
    playerHealthBar_->Update();

    //==================================================
    // プレイヤーの経験値ゲージ
    //==================================================

    playerExpBar_->SetExpCurrent(param.exp);
    playerExpBar_->SetExpMax(param.expToNextLevel);

    //==================================================
    // プレイヤーのレベルテキスト
    //==================================================

    std::string levelText = "Lv." + std::to_string(param.level);
    playerLevelText_->SetText(std::u8string(levelText.begin(), levelText.end()));
}

void GameScene::Draw() {
    player_->Draw();
    for (const auto& enemy : enemies_) {
        enemy->Draw();
    }
    for (const auto& bullet : bullets_) {
        bullet->Draw();
    }
    for (const auto& particle : backgroundParticleBlocks_) {
        particle->Draw();
    }
    for (const auto &particle : deathParticles_) {
        particle->Draw();
    }
    playerHealthBar_->Draw();
    playerExpBar_->Draw();
    playerLevelText_->SetPipelineName("Object3d.Solid.BlendNormal");
    playerLevelText_->Draw();
    playerLevelText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    playerLevelText_->Draw();
    playerBulletTypeText_->Draw();
    bulletChangePromptText_->SetPipelineName("Object3d.Solid.BlendNormal");
    bulletChangePromptText_->Draw();
    bulletChangePromptText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    bulletChangePromptText_->Draw();
    for (const auto& text : damageAmountDisplays_) {
        text->Draw();
    }
    playerLevelUpText_->Draw();
    playerShootCoolDownBar_->Draw();
    laserPointer_->Draw();
    gridLineFloor_->Draw();
    gridLineLeftWall_->Draw();
    gridLineRightWall_->Draw();
    waveDisplay_->Draw();
    waveDrawAnimation_->Draw();
    resultDrawAnimation_->Draw();
    returnToTitlePromptText_->SetPipelineName("Object3d.Solid.BlendNormal");
    returnToTitlePromptText_->Draw();
    returnToTitlePromptText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    returnToTitlePromptText_->Draw();
    pauseMenu_->Draw();
    openRectToCenterY_->Draw();
    closeRectToCenterY_->Draw();
    fadeOut_->Draw();
}

void GameScene::CheckAllCollisions() {
    collisionManager_->ClearColliders();
    if (player_->IsAlive()) {
        collisionManager_->AddCollider(player_.get());
    }
    for (const auto& enemy : enemies_) {
        collisionManager_->AddCollider(enemy.get());
    }
    for (const auto& bullet : bullets_) {
        collisionManager_->AddCollider(bullet.get());
    }
    collisionManager_->Update();
}
