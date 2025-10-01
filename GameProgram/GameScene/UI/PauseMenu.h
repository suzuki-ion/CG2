#pragma once
#include <Objects/Text.h>
#include <Objects/Sprite.h>

enum class PauseMenuButton {
    kContinue,
    kRestart,
    kBackTitle,
    kExit
};

class PauseMenu {
public:
    PauseMenu() = delete;
    PauseMenu(float width, float height);
    ~PauseMenu() = default;

    void SelectUp();
    void SelectDown();
    void Confirm();

    void Open() {
        selectedButton_ = PauseMenuButton::kContinue;
        SetColor();
        isActive_ = true;
    }
    void Close() {
        isActive_ = false;
    }

    PauseMenuButton GetSelectedButton() const { return selectedButton_; }
    bool IsActive() const { return isActive_; }

    void Update();
    void Draw() const;

private:
    void SetColor();

    std::unique_ptr<KashipanEngine::Text> logoText_;
    std::unique_ptr<KashipanEngine::Text> continueText_;
    std::unique_ptr<KashipanEngine::Text> restartText_;
    std::unique_ptr<KashipanEngine::Text> backTitleText_;
    std::unique_ptr<KashipanEngine::Text> exitText_;
    std::unique_ptr<KashipanEngine::Sprite> backgroundSprite_;

    KashipanEngine::Vector4 selectColor_ = KashipanEngine::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    KashipanEngine::Vector4 nonSelectColor_ = KashipanEngine::Vector4(0.5f, 0.5f, 0.5f, 1.0f);
    KashipanEngine::Vector4 confirmColor_ = KashipanEngine::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    float logoAnimationAngle_ = 0.0f;
    PauseMenuButton selectedButton_ = PauseMenuButton::kContinue;
    bool isActive_ = false;
};