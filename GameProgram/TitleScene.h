#pragma once
#include <Common/SceneBase.h>
#include <Common/KeyConfig.h>
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include "SceneChangeAnimation/OpenRectToCenterY.h"
#include "SceneChangeAnimation/CloseRectToCenterY.h"

class TitleScene : public KashipanEngine::SceneBase {
public:
    TitleScene();
    ~TitleScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Update() override;
    void Draw() override;

private:
    // シーン切り替えアニメーションオブジェクト(In)
    std::unique_ptr<OpenRectToCenterY> sceneChangeAnimationIn_;
    // シーン切り替えアニメーションオブジェクト(Out)
    std::unique_ptr<CloseRectToCenterY> sceneChangeAnimationOut_;

    // タイトルロゴのテキストオブジェクト
    std::unique_ptr<KashipanEngine::Text> titleLogoText_;
    // タイトルの開始を促すテキストオブジェクト
    std::unique_ptr<KashipanEngine::Text> startPromptText_;
    // 背景用のスプライトオブジェクト
    std::unique_ptr<KashipanEngine::Sprite> backgroundSprite_;

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