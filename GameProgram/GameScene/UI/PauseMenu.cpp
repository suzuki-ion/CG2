#include "PauseMenu.h"
#include <KashipanEngine.h>
#include <cmath>

using namespace KashipanEngine;

PauseMenu::PauseMenu(float width, float height) {
    logoText_ = std::make_unique<Text>(16);
    continueText_ = std::make_unique<Text>(16);
    restartText_ = std::make_unique<Text>(16);
    backTitleText_ = std::make_unique<Text>(16);
    exitText_ = std::make_unique<Text>(16);

    logoText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    continueText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    restartText_->SetFont(continueText_->GetFontData());
    backTitleText_->SetFont(continueText_->GetFontData());
    exitText_->SetFont(continueText_->GetFontData());

    logoText_->SetText(u8"ポーズ");
    continueText_->SetText(u8"つづける");
    restartText_->SetText(u8"さいしょから");
    backTitleText_->SetText(u8"タイトルにもどる");
    exitText_->SetText(u8"ゲームをやめる");

    logoText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    continueText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    restartText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    backTitleText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    exitText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    float centerX = width / 2.0f;
    logoText_->GetStatePtr().transform->translate = Vector3(centerX, height / 4.0f, 0.0f);
    continueText_->GetStatePtr().transform->translate = Vector3(centerX, height / 2.0f, 0.0f);
    restartText_->GetStatePtr().transform->translate = Vector3(centerX, height / 2.0f + 128.0f, 0.0f);
    backTitleText_->GetStatePtr().transform->translate = Vector3(centerX, height / 2.0f + 256.0f, 0.0f);
    exitText_->GetStatePtr().transform->translate = Vector3(centerX, height / 2.0f + 384.0f, 0.0f);

    backgroundSprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    backgroundSprite_->GetStatePtr().transform->scale = Vector3(width, height, 1.0f);
    backgroundSprite_->GetStatePtr().material->color = Vector4(0.0f, 0.0f, 0.0f, 0.75f);

    SetColor();
}

void PauseMenu::SelectUp() {
    if (!isActive_) {
        return;
    }
    int selectButton = static_cast<int>(selectedButton_);
    selectButton--;
    if (selectButton < 0) {
        selectButton = static_cast<int>(PauseMenuButton::kExit);
    }
    selectedButton_ = static_cast<PauseMenuButton>(selectButton);
    SetColor();
}

void PauseMenu::SelectDown() {
    if (!isActive_) {
        return;
    }
    int selectButton = static_cast<int>(selectedButton_);
    selectButton++;
    if (selectButton > static_cast<int>(PauseMenuButton::kExit)) {
        selectButton = 0;
    }
    selectedButton_ = static_cast<PauseMenuButton>(selectButton);
    SetColor();
}

void PauseMenu::Confirm() {
    if (!isActive_) {
        return;
    }
    switch (selectedButton_) {
        case PauseMenuButton::kContinue:
            continueText_->GetStatePtr().material->color = confirmColor_;
            break;
        case PauseMenuButton::kRestart:
            restartText_->GetStatePtr().material->color = confirmColor_;
            break;
        case PauseMenuButton::kBackTitle:
            backTitleText_->GetStatePtr().material->color = confirmColor_;
            break;
        case PauseMenuButton::kExit:
            exitText_->GetStatePtr().material->color = confirmColor_;
            break;
        default:
            break;
    }
}

void PauseMenu::Update() {
    if (!isActive_) {
        return;
    }
    // タイトルロゴだけアニメーションさせる
    logoAnimationAngle_ += Engine::GetDeltaTime() * 2.0f;
    logoText_->GetStatePtr().transform->translate.y = std::sin(logoAnimationAngle_) * 16.0f + 1080.0f / 4.0f;
}

void PauseMenu::Draw() const {
    if (!isActive_) {
        return;
    }
    backgroundSprite_->Draw();
    logoText_->Draw();
    continueText_->Draw();
    restartText_->Draw();
    backTitleText_->Draw();
    exitText_->Draw();
}

void PauseMenu::SetColor() {
    continueText_->GetStatePtr().material->color =
        (selectedButton_ == PauseMenuButton::kContinue) ? selectColor_ : nonSelectColor_;
    restartText_->GetStatePtr().material->color =
        (selectedButton_ == PauseMenuButton::kRestart) ? selectColor_ : nonSelectColor_;
    backTitleText_->GetStatePtr().material->color =
        (selectedButton_ == PauseMenuButton::kBackTitle) ? selectColor_ : nonSelectColor_;
    exitText_->GetStatePtr().material->color =
        (selectedButton_ == PauseMenuButton::kExit) ? selectColor_ : nonSelectColor_;
}
