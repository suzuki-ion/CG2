#include <Base/WinApp.h>
#include <Base/DirectXCommon.h>
#include <Base/Input.h>
#include <Base/SceneManager.h>
#include <Base/Sound.h>
#include <Base/Texture.h>
#include <Base/Renderer.h>
#include <Common/ConvertString.h>
#include <Common/Random.h>
#include "TitleScene.h"

using namespace KashipanEngine;

TitleScene::TitleScene() : SceneBase("TitleScene") {
    engine_->GetDxCommon()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    // カメラの生成
    camera_ = std::make_unique<Camera>();
    camera_->SetTranslate(Vector3(-6.0f, 10.0f, 70.0f));

    CameraPerspective cameraPerspective;
    cameraPerspective.fovY = 0.4f;
    float width = static_cast<float>(engine_->GetWinApp()->GetClientWidth());
    float height = static_cast<float>(engine_->GetWinApp()->GetClientHeight());
    cameraPerspective.aspectRatio = width / height;
    cameraPerspective.nearClip = 0.1f;
    cameraPerspective.farClip = 64.0f;

    CameraViewport cameraViewport;
    cameraViewport.left = 0.0f;
    cameraViewport.top = 0.0f;
    cameraViewport.width = width;
    cameraViewport.height = height;

    camera_->SetCameraPerspective(cameraPerspective);
    camera_->SetCameraViewport(cameraViewport);

    Vector3 target = { 0.0f, 8.0f, 64.0f };
    Vector3 direction = (camera_->GetTranslate() - target).Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    camera_->SetRotate(Vector3(pitch, -yaw + -0.15f, 0.0f));

    //==================================================
    // モデル生成
    //==================================================

    // プレイヤーのモデル生成
    playerModel_ = std::make_unique<Model>("Resources/Player", "player.obj");
    auto playerState = playerModel_->GetStatePtr();
    playerState.transform->translate = target;
    playerState.material->lightingType = 0;
    playerState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    // 背景パーティクル用モデル生成
    backgroundParticleModel_ = std::make_unique<Model>("Resources/Block", "block.obj");
    backgroundParticleModel_->SetPipelineName("Object3d.DoubleSidedCulling.BlendNormal");
    auto particleBlockGreenState = backgroundParticleModel_->GetStatePtr();
    particleBlockGreenState.material->lightingType = 0;
    particleBlockGreenState.material->color = Vector4(0.0f, 0.5f, 0.0f, 1.0f);

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

    sceneChangeAnimationIn_ = std::make_unique<OpenRectToCenterY>(1.0f, 1920.0f, 1080.0f);
    sceneChangeAnimationOut_ = std::make_unique<CloseRectToCenterY>(1.0f, 1920.0f, 1080.0f);

    titleLogoText_ = std::make_unique<Text>(32);
    titleLogoText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    titleLogoText_->SetText(u8"バグ撃");
    titleLogoText_->SetTextAlign(TextAlignX::Left, TextAlignY::Center);

    auto titleLogoState = titleLogoText_->GetStatePtr();
    titleLogoState.transform->translate.x = 128.0f;
    titleLogoState.transform->translate.y = 1080.0f / 3.0f;
    titleLogoState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    startPromptText_ = std::make_unique<Text>(32);
    startPromptText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    startPromptText_->SetText(u8"スペースキーでスタート");
    startPromptText_->SetTextAlign(TextAlignX::Left, TextAlignY::Center);

    auto startPromptState = startPromptText_->GetStatePtr();
    startPromptState.transform->translate.x = 128.0f;
    startPromptState.transform->translate.y = 1080.0f / 4.0f * 3.0f;
    startPromptState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    // UI操作用のキーコンフィグを設定
    uiKeyConfig_.LoadFromJson("Resources/KeyConfig/UISelectKeyConfig.json");

    // BGMを読み込み
    bgmSoundIndex_ = Sound::Load("Resources/BGM/title.mp3");
    // 決定音を読み込み
    confirmSoundIndex_ = Sound::Load("Resources/SE/titleConfirm.mp3");
};

void TitleScene::Initialize() {
    engine_->GetRenderer()->SetCamera(camera_.get());

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

    Sound::Play(bgmSoundIndex_, 0.5f, 0.0f, true);
    sceneChangeAnimationOut_->Stop();
    sceneChangeAnimationIn_->Play();
    animationAngle_ = 0.0f;
    isInitialized_ = true;
}

void TitleScene::Finalize() {
    Sound::Stop(bgmSoundIndex_);
    isInitialized_ = false;
}

void TitleScene::Update() {
    if (sceneChangeAnimationOut_->IsFinished()) {
        SceneManager::SetActiveScene("GameScene");
        return;
    }
    if (sceneChangeAnimationIn_->IsFinished() &&
        !sceneChangeAnimationOut_->IsPlaying() &&
        !sceneChangeAnimationOut_->IsFinished()) {
        if (std::get<bool>(uiKeyConfig_.GetInputValue("SelectConfirm"))) {
            Sound::Play(confirmSoundIndex_, 0.5f, 0.0f, false);
            sceneChangeAnimationOut_->Play();
        }
    }

    // 入力デバイスが変わったらテキストを更新
    InputDeviceType nowInputDevice = Input::GetCurrentInputDeviceType();
    if (nowInputDevice != InputDeviceType::None &&
        currentInputDevice_ != nowInputDevice) {
        std::u8string newPromptText;
        switch (nowInputDevice) {
            case KashipanEngine::InputDeviceType::None:
                break;
            case KashipanEngine::InputDeviceType::Keyboard:
            case KashipanEngine::InputDeviceType::Mouse:
                newPromptText = u8"スペースキーでスタート";
                break;
            case KashipanEngine::InputDeviceType::XBoxController:
                newPromptText = u8"Aボタンでスタート";
                break;
            default:
                break;
        }
        startPromptText_->SetText(newPromptText);
        currentInputDevice_ = nowInputDevice;
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
    // 背景用ブロックパーティクル
    //==================================================

    for (const auto &particle : backgroundParticleBlocks_) {
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

    auto playerState = playerModel_->GetStatePtr();
    animationAngle_ += Engine::GetDeltaTime() * 2.0f;
    playerState.transform->translate.y = std::sin(animationAngle_) * 0.2f + 8.0f;

    sceneChangeAnimationIn_->Update();
    sceneChangeAnimationOut_->Update();
}

void TitleScene::Draw() {
    playerModel_->Draw();
    for (const auto &particle : backgroundParticleBlocks_) {
        particle->Draw();
    }
    gridLineFloor_->Draw();
    gridLineLeftWall_->Draw();
    gridLineRightWall_->Draw();

    titleLogoText_->SetPipelineName("Object3d.Solid.BlendNormal");
    titleLogoText_->Draw();
    titleLogoText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    titleLogoText_->Draw();
    
    startPromptText_->Draw();

    sceneChangeAnimationIn_->Draw();
    sceneChangeAnimationOut_->Draw();
}
