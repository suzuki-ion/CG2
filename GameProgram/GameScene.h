#pragma once
#include <Common/SceneBase.h>
#include <Common/KeyConfig.h>
#include <Objects/Plane.h>
#include <Objects/Cube.h>
#include <Math/Camera.h>

#include "GameScene/Player.h"
#include "GameScene/CollisionManager.h"
#include "GameScene/ResultParameters.h"

#include "GameScene/Particle/DeathParticle.h"
#include "GameScene/HUD/PlayerHealthIcons.h"
#include "GameScene/HUD/WaveDisplay.h"
#include "GameScene/UI/PauseMenu.h"

#include "SceneChangeAnimation/OpenRectToCenterY.h"
#include "SceneChangeAnimation/CloseRectToCenterY.h"
#include "SceneChangeAnimation/FadeIn.h"
#include "SceneChangeAnimation/FadeOut.h"

class GameScene : public KashipanEngine::SceneBase {
public:
    GameScene();
    ~GameScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Update() override;
    void Draw() override;

    void AddEnemy(std::unique_ptr<BaseEntity> enemy) {
        std::list<BaseEntity *> player;
        player.push_back(player_.get());
        enemy->SetTargetList(player);
        enemy->SetAddBulletFunction([this](std::unique_ptr<BaseBullet> bullet) { AddBullet(std::move(bullet)); });
        enemies_.emplace_back(std::move(enemy));
    }
    void AddBullet(std::unique_ptr<BaseBullet> bullet) {
        if (bullet->GetCollisionAttribute() == kCollisionAttributePlayer) {
            bullet->SetIsExistTargetFunc([this](BaseEntity *target) { return IsExistEnemy(target); });
        } else if (bullet->GetCollisionAttribute() == kCollisionAttributeEnemy) {
            bullet->SetIsExistTargetFunc([this](BaseEntity *target) { return IsExistPlayer(target); });
        }
        bullets_.emplace_back(std::move(bullet));
    }
    void AddDeathParticle(std::unique_ptr<DeathParticle> particle) {
        deathParticles_.emplace_back(std::move(particle));
    }

    bool IsExistEnemy(BaseEntity *enemy) {
        for (const auto& e : enemies_) {
            if (e.get() == enemy) {
                return true;
            }
        }
        return false;
    }
    bool IsExistPlayer(BaseEntity *enemy) const {
        static_cast<void>(enemy); // 未使用引数対策
        return player_ ->IsAlive();
    }

private:
    void UpdateInputDevice();
    void UpdateSceneChangeAnimation();
    void UpdatePauseMenu();
    void UpdatePlayer();
    void UpdateEnemy();
    void UpdateBullet();
    void UpdateCollision();
    void UpdateDeathParticles();
    void UpdatePlayerHealthIcons();

    // 当たり判定用関数
    void CheckAllCollisions();

    bool IsPauseCondition();
    bool IsPlayerMoveCondition();

    // カメラ
    std::unique_ptr<KashipanEngine::Camera> camera2D_;

    //==================================================
    // 定数
    //==================================================

    // BGMへのインデックス
    int bgmIndex_ = -1;

    // 弾発射SEへのインデックス
    int seShootIndex_ = -1;
    // 弾命中SEへのインデックス
    int seHitIndex_ = -1;
    // 殺害SEへのインデックス
    int seKillIndex_ = -1;
    // ダメージSEへのインデックス
    int seDamageIndex_ = -1;

    // ウィンドウ移動SEへのインデックス
    int seWindowMoveIndex_ = -1;
    // ウィンドウ展開SEへのインデックス
    int seWindowOpenIndex_ = -1;
    
    // 現在の入力デバイス
    KashipanEngine::InputDeviceType currentInputDevice_ =
        KashipanEngine::InputDeviceType::Keyboard;

    //==================================================
    // スプライト
    //==================================================

    // 背景用スプライト
    std::unique_ptr<KashipanEngine::Sprite> backgroundSprite_;

    // プレイヤーのスプライト
    std::unique_ptr<KashipanEngine::Sprite> playerSprite_;
    // 敵のスプライト
    std::unique_ptr<KashipanEngine::Sprite> enemyNormalSprite_;
    
    // プレイヤーの弾(通常)のスプライト
    std::unique_ptr<KashipanEngine::Sprite> playerBulletNormalSprite_;
    // プレイヤーの弾(散弾)のスプライト
    std::unique_ptr<KashipanEngine::Sprite> playerBulletShotgunSprite_;
    // プレイヤーの弾(レーザー)のスプライト
    std::unique_ptr<KashipanEngine::Sprite> playerBulletLaserSprite_;

    // 死亡パーティクル用スプライト
    std::unique_ptr<KashipanEngine::Model> deathParticleModel_;

    // デフォルトのテキストのフォントデータ
    std::unique_ptr<KashipanEngine::Text> defaultTextFontData_;

    //==================================================
    // インスタンス
    //==================================================

    // プレイヤー
    std::unique_ptr<Player> player_;
    // 敵のリスト
    std::list<std::unique_ptr<BaseEntity>> enemies_;
    // 弾のリスト
    std::list<std::unique_ptr<BaseBullet>> bullets_;

    // 死亡パーティクルのリスト
    std::list<std::unique_ptr<DeathParticle>> deathParticles_;

    // プレイヤーの体力アイコン
    std::unique_ptr<PlayerHealthIcons> playerHealthIcons_;
    // ポーズメニュー
    std::unique_ptr<PauseMenu> pauseMenu_;
    
    // プレイヤーのキー設定
    KashipanEngine::KeyConfig playerKeyConfig_;
    // UI操作用のキーコンフィグ
    KashipanEngine::KeyConfig uiKeyConfig_;

    // 当たり判定管理
    std::unique_ptr<CollisionManager> collisionManager_;

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    // 切り替えるシーンの名前
    std::string nextSceneName_;
    // シーン開始アニメーション
    std::unique_ptr<FadeIn> sceneStartFadeIn_;
    // シーン終了アニメーション
    std::unique_ptr<FadeOut> sceneEndFadeOut_;
};