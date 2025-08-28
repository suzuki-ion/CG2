#pragma once
#include <Common/SceneBase.h>
#include <Common/KeyConfig.h>
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include "SceneChangeAnimation/FadeIn.h"
#include "SceneChangeAnimation/FadeOut.h"

class GameOverScene : public KashipanEngine::SceneBase {
public:
    GameOverScene();
    ~GameOverScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Update() override;
    void Draw() override;

private:
    // シーン切り替えアニメーションオブジェクト(In)
    std::unique_ptr<FadeIn> sceneChangeAnimationIn_;
    // シーン切り替えアニメーションオブジェクト(Out)
    std::unique_ptr<FadeOut> sceneChangeAnimationOut_;

    // ゲームオーバーロゴオブジェクト
    std::unique_ptr<KashipanEngine::Text> gameOverLogoText_;
    // タイトルに戻るキーのテキストオブジェクト
    std::unique_ptr<KashipanEngine::Text> backTitlePromptText_;

    // UI操作用のキーコンフィグ
    KashipanEngine::KeyConfig uiKeyConfig_;

    // 現在の入力デバイス
    KashipanEngine::InputDeviceType currentInputDevice_ =
        KashipanEngine::InputDeviceType::Keyboard;

    float animationAngle_ = 0.0f; // アニメーション用の角度

    // BGMの音声インデックス
    int bgmSoundIndex_ = -1;
    // 決定音の音声インデックス
    int confirmSoundIndex_ = -1;
};