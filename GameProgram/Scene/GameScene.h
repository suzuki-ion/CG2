#pragma once
#include <Common/SceneBase.h>
#include <Common/KeyConfig.h>
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include <Objects/Model.h>
#include <Objects/Plane.h>
#include <Math/Camera.h>
#include "SceneChangeAnimation/OpenRectToCenterY.h"
#include "SceneChangeAnimation/CloseRectToCenterY.h"
#include "GameScene/Particle/ParticleBlock.h"

class GameScene : public KashipanEngine::SceneBase {
public:
    GameScene();
    ~GameScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Update() override;
    void Draw() override;

private:
    // カメラ
    std::unique_ptr<KashipanEngine::Camera> camera_;
    // 背景用のブロックパーティクルの数
    const int kBackgroundParticleBlockCount_ = 128;

    // シーン切り替えアニメーションオブジェクト(In)
    std::unique_ptr<OpenRectToCenterY> sceneChangeAnimationIn_;
    // シーン切り替えアニメーションオブジェクト(Out)
    std::unique_ptr<CloseRectToCenterY> sceneChangeAnimationOut_;

    //==================================================
    // モデル
    //==================================================

    // プレイヤーのモデル
    std::unique_ptr<KashipanEngine::Model> playerModel_;
    // 背景パーティクル用モデル
    std::unique_ptr<KashipanEngine::Model> backgroundParticleModel_;
    // グリッドライン(床)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineFloor_;
    // グリッドライン(左壁)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineLeftWall_;
    // グリッドライン(右壁)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineRightWall_;

    // タイトルロゴのテキストオブジェクト
    std::unique_ptr<KashipanEngine::Text> titleLogoText_;
    // タイトルの開始を促すテキストオブジェクト
    std::unique_ptr<KashipanEngine::Text> startPromptText_;
    // 背景用のブロックパーティクルのリスト
    std::list<std::unique_ptr<ParticleBlock>> backgroundParticleBlocks_;

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