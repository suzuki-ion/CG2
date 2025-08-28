#include <Base/DirectXCommon.h>
#include <Base/Input.h>
#include <Base/SceneManager.h>
#include <Base/Sound.h>
#include <Common/ConvertString.h>
#include "GameOverScene.h"

using namespace KashipanEngine;

GameOverScene::GameOverScene() : SceneBase("GameOverScene") {
    engine_->GetDxCommon()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    sceneChangeAnimationIn_ = std::make_unique<FadeIn>(1.0f, Vector4(0.0f), 1920.0f, 1080.0f);
    sceneChangeAnimationOut_ = std::make_unique<FadeOut>(1.0f, Vector4(0.0f), 1920.0f, 1080.0f);

    gameOverLogoText_ = std::make_unique<Text>(32);
    gameOverLogoText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    gameOverLogoText_->SetText(u8"ゲームオーバー");
    gameOverLogoText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    auto gameOverLogoState = gameOverLogoText_->GetStatePtr();
    gameOverLogoState.transform->translate.x = 1920.0f / 2.0f;
    gameOverLogoState.transform->translate.y = 1080.0f / 3.0f;

    backTitlePromptText_ = std::make_unique<Text>(32);
    backTitlePromptText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    backTitlePromptText_->SetText(u8"スペースキーでタイトルにもどる");
    backTitlePromptText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    auto backTitlePromptState = backTitlePromptText_->GetStatePtr();
    backTitlePromptState.transform->translate.x = 1920.0f / 2.0f;
    backTitlePromptState.transform->translate.y = 1080.0f / 3.0f * 2.0f;

    // UI操作用のキーコンフィグを設定
    uiKeyConfig_.LoadFromJson("Resources/KeyConfig/UISelectKeyConfig.json");

    // BGMを読み込み
    bgmSoundIndex_ = Sound::Load("Resources/BGM/gameOver.mp3");
    // 決定音を読み込み
    confirmSoundIndex_ = Sound::Load("Resources/SE/uiConfirm.mp3");
};

void GameOverScene::Initialize() {
    Sound::Play(bgmSoundIndex_, 0.5f, 0.0f, true);
    sceneChangeAnimationOut_->Stop();
    sceneChangeAnimationIn_->Play();
    animationAngle_ = 0.0f;
    isInitialized_ = true;
}

void GameOverScene::Finalize() {
    Sound::Stop(bgmSoundIndex_);
    isInitialized_ = false;
}

void GameOverScene::Update() {
    if (sceneChangeAnimationOut_->IsFinished()) {
        SceneManager::SetActiveScene("TitleScene");
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
                newPromptText = u8"スペースキーでタイトルにもどる";
                break;
            case KashipanEngine::InputDeviceType::XBoxController:
                newPromptText = u8"Aボタンでタイトルにもどる";
                break;
            default:
                break;
        }
        backTitlePromptText_->SetText(newPromptText);
        currentInputDevice_ = nowInputDevice;
    }

    auto gameOverLogoState = gameOverLogoText_->GetStatePtr();
    animationAngle_ += Engine::GetDeltaTime();
    gameOverLogoState.transform->translate.y = std::sin(animationAngle_) * 32.0f + (1080.0f / 3.0f);
    if (!sceneChangeAnimationOut_->IsPlaying()) {
        if (std::fmodf(animationAngle_, 1.0f) < 0.5f) {
            backTitlePromptText_->GetStatePtr().material->color.w = 1.0f;
        } else {
            backTitlePromptText_->GetStatePtr().material->color.w = 0.0f;
        }
    } else {
        if (std::fmodf(animationAngle_, 0.25f) < 0.125f) {
            backTitlePromptText_->GetStatePtr().material->color.w = 1.0f;
        } else {
            backTitlePromptText_->GetStatePtr().material->color.w = 0.0f;
        }
    }

    sceneChangeAnimationIn_->Update();
    sceneChangeAnimationOut_->Update();
}

void GameOverScene::Draw() {
    gameOverLogoText_->Draw();
    backTitlePromptText_->Draw();

    sceneChangeAnimationIn_->Draw();
    sceneChangeAnimationOut_->Draw();
}
