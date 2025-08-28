#include <Base/DirectXCommon.h>
#include <Base/Input.h>
#include <Base/SceneManager.h>
#include <Base/Sound.h>
#include <Common/ConvertString.h>
#include "TitleScene.h"

using namespace KashipanEngine;

TitleScene::TitleScene() : SceneBase("TitleScene") {
    engine_->GetDxCommon()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    sceneChangeAnimationIn_ = std::make_unique<OpenRectToCenterY>(1.0f, 1920.0f, 1080.0f);
    sceneChangeAnimationOut_ = std::make_unique<CloseRectToCenterY>(1.0f, 1920.0f, 1080.0f);

    titleLogoText_ = std::make_unique<Text>(32);
    titleLogoText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    titleLogoText_->SetText(u8"バグ撃");
    titleLogoText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    auto titleLogoState = titleLogoText_->GetStatePtr();
    titleLogoState.transform->translate.x = 1920.0f / 2.0f;
    titleLogoState.transform->translate.y = 1080.0f / 3.0f;
    titleLogoState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    startPromptText_ = std::make_unique<Text>(32);
    startPromptText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    startPromptText_->SetText(u8"スペースキーでスタート");
    startPromptText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    auto startPromptState = startPromptText_->GetStatePtr();
    startPromptState.transform->translate.x = 1920.0f / 2.0f;
    startPromptState.transform->translate.y = 1080.0f / 3.0f * 2.0f;
    startPromptState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

    // 背景用のスプライトを作成
    backgroundSprite_ = std::make_unique<Sprite>("Resources/grid.png");
    auto bgState = backgroundSprite_->GetStatePtr();
    bgState.uvTransform->scale = Vector2(2.0f, 2.0f);
    bgState.transform->scale = Vector3(2.0f, 2.0f, 1.0f);
    bgState.material->color = Vector4(0.0f, 0.125f, 0.0f, 1.0f);

    // UI操作用のキーコンフィグを設定
    uiKeyConfig_.LoadFromJson("Resources/KeyConfig/UISelectKeyConfig.json");

    // BGMを読み込み
    bgmSoundIndex_ = Sound::Load("Resources/BGM/title.mp3");
    // 決定音を読み込み
    confirmSoundIndex_ = Sound::Load("Resources/SE/titleConfirm.mp3");
};

void TitleScene::Initialize() {
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

    auto titleLogoState = titleLogoText_->GetStatePtr();
    animationAngle_ += Engine::GetDeltaTime() * 4.0f;
    titleLogoState.transform->translate.y = std::sin(animationAngle_) * 32.0f + (1080.0f / 3.0f);

    auto bgState = backgroundSprite_->GetStatePtr();
    bgState.uvTransform->translate.x -= 0.05f * Engine::GetDeltaTime();
    bgState.uvTransform->translate.y -= 0.05f * Engine::GetDeltaTime();

    sceneChangeAnimationIn_->Update();
    sceneChangeAnimationOut_->Update();
}

void TitleScene::Draw() {
    backgroundSprite_->Draw();

    titleLogoText_->SetPipelineName("Object3d.Solid.BlendNormal");
    titleLogoText_->Draw();
    titleLogoText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    titleLogoText_->Draw();
    
    startPromptText_->Draw();

    sceneChangeAnimationIn_->Draw();
    sceneChangeAnimationOut_->Draw();
}
