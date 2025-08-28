#pragma once
#include <Common/SceneBase.h>
#include <Common/KeyConfig.h>
#include <Objects/Plane.h>
#include <Objects/Cube.h>
#include <Math/Camera.h>

#include "GameScene/Player.h"
#include "GameScene/CollisionManager.h"
#include "GameScene/WaveManager.h"
#include "GameScene/WaveDrawAnimation.h"
#include "GameScene/ResultDrawAnimation.h"

#include "GameScene/Particle/DeathParticle.h"
#include "GameScene/HUD/PlayerHealthBar.h"
#include "GameScene/HUD/PlayerExpBar.h"
#include "GameScene/HUD/PlayerBulletTypeText.h"
#include "GameScene/HUD/LaserPointer.h"
#include "GameScene/HUD/DamageAmountDisplay.h"
#include "GameScene/HUD/WaveDisplay.h"
#include "GameScene/HUD/PlayerLevelUpText.h"
#include "GameScene/HUD/PlayerShootCoolDownBar.h"
#include "GameScene/UI/PauseMenu.h"

#include "SceneChangeAnimation/OpenRectToCenterY.h"
#include "SceneChangeAnimation/CloseRectToCenterY.h"
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
        enemy->SetShootFunction([this](std::unique_ptr<BaseBullet> bullet) { AddBullet(std::move(bullet)); });
        enemy->SetShowDamageFunction([this](float damage, const KashipanEngine::Vector3 &position) { AddDamageAmountDisplay(damage, position); });
        enemy->SetResultAddDamageFunction([this](float damage) { AddTotalDamage(damage); });
        enemy->SetResultAddKillCountFunction([this]() { AddKillCount(); });
        enemies_.emplace_back(std::move(enemy));
    }
    void AddBullet(std::unique_ptr<BaseBullet> bullet) {
        if (bullet->GetCollisionAttribute() == kCollisionAttributePlayer) {
            bullet->SetIsExistTargetFunc([this](BaseEntity *target) { return IsExistEnemy(target); });
        } else if (bullet->GetCollisionAttribute() == kCollisionAttributeEnemy) {
            bullet->SetIsExistTargetFunc([this](BaseEntity *target) { return IsExistPlayer(target); });
        }
        bullet->SetShowDamageFunction([this](float damage, const KashipanEngine::Vector3& position) { AddDamageAmountDisplay(damage, position); });
        bullets_.emplace_back(std::move(bullet));
    }
    void AddDeathParticle(std::unique_ptr<DeathParticle> particle) {
        deathParticles_.emplace_back(std::move(particle));
    }
    void AddDamageAmountDisplay(float damage, const KashipanEngine::Vector3& position) {
        damageAmountDisplays_.emplace_back(
            std::make_unique<DamageAmountDisplay>(damage, position, camera_.get(), damageAmountText_->GetFontData()));
    }

    void AddKillCount() {
        resultParameters_.killCount++;
    }
    void AddTotalDamage(float damage) {
        resultParameters_.totalDamage += damage;
    }
    void AddDamageTaken(float damage) {
        resultParameters_.damageTaken += damage;
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
    // 当たり判定用関数
    void CheckAllCollisions();

    // カメラ
    std::unique_ptr<KashipanEngine::Camera> camera_;

    //==================================================
    // 定数
    //==================================================

    // BGM(通常ウェーブ)へのインデックス
    int bgmNormalWaveIndex_ = -1;
    // BGM(ラストウェーブ)へのインデックス
    int bgmLastWaveIndex_ = -1;
    // BGm(ウェーブクリア)へのインデックス
    int bgmClearWaveIndex_ = -1;

    // SE(プレイヤーレベルアップ)へのインデックス
    int seLevelUpIndex_ = -1;
    // SE(弾発射)へのインデックス
    int seShootIndex_ = -1;
    // SE(ダメージ)へのインデックス
    int seDamageIndex_ = -1;
    // SE(死亡)へのインデックス
    int seDeathIndex_ = -1;
    // SE(通常ウェーブ開始)へのインデックス
    int seNormalWaveStartIndex_ = -1;
    // SE(ラストウェーブ開始)へのインデックス
    int seLastWaveStartIndex_ = -1;
    
    // SE(UI選択)へのインデックス
    int seUISelectIndex_ = -1;
    // SE(UI決定)へのインデックス
    int seUIConfirmIndex_ = -1;
    // SE(UIキャンセル)へのインデックス
    int seUICancelIndex_ = -1;
    // SE(リザルト時の決定)へのインデックス
    int seResultConfirmIndex_ = -1;

    // 背景用のブロックパーティクルの数
    const int kBackgroundParticleBlockCount_ = 128;

    // 現在の入力デバイス
    KashipanEngine::InputDeviceType currentInputDevice_ =
        KashipanEngine::InputDeviceType::Keyboard;

    // リザルト用の情報
    ResultParameters resultParameters_;

    //==================================================
    // モデル
    //==================================================

    // プレイヤーのモデル
    std::unique_ptr<KashipanEngine::Model> playerModel_;
    // 敵のモデル
    std::unique_ptr<KashipanEngine::Model> enemyModel_;
    
    // プレイヤーの弾のモデル
    std::unique_ptr<KashipanEngine::Model> playerBulletModel_;
    // 敵の弾のモデル
    std::unique_ptr<KashipanEngine::Model> enemyBulletModel_;

    // 背景パーティクル用モデル
    std::unique_ptr<KashipanEngine::Model> backgroundParticleModel_;
    // 死亡パーティクル用モデル
    std::unique_ptr<KashipanEngine::Model> deathParticleModel_;

    // グリッドライン(床)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineFloor_;
    // グリッドライン(左壁)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineLeftWall_;
    // グリッドライン(右壁)の板ポリ
    std::unique_ptr<KashipanEngine::Plane> gridLineRightWall_;

    // ダメージ量表示用のテキスト
    std::unique_ptr<KashipanEngine::Text> damageAmountText_;
    // 弾切り替え方法表示用テキスト
    std::unique_ptr<KashipanEngine::Text> bulletChangePromptText_ ;
    // リザルト時タイトルに戻る方法表示用テキスト
    std::unique_ptr<KashipanEngine::Text> returnToTitlePromptText_;
    // プレイヤーのレベル表示用テキスト
    std::unique_ptr<KashipanEngine::Text> playerLevelText_;

    //==================================================
    // インスタンス
    //==================================================

    // プレイヤー
    std::unique_ptr<Player> player_;
    // 敵のリスト
    std::list<std::unique_ptr<BaseEntity>> enemies_;
    // 弾のリスト
    std::list<std::unique_ptr<BaseBullet>> bullets_;

    // 背景用のブロックパーティクルのリスト
    std::list<std::unique_ptr<ParticleBlock>> backgroundParticleBlocks_;
    // 死亡パーティクルのリスト
    std::list<std::unique_ptr<DeathParticle>> deathParticles_;
    // レーザーポインター
    std::unique_ptr<LaserPointer> laserPointer_;

    // プレイヤーの体力ゲージ
    std::unique_ptr<PlayerHealthBar> playerHealthBar_;
    // プレイヤーの経験値ゲージ
    std::unique_ptr<PlayerExpBar> playerExpBar_;
    // プレイヤーの使用してる弾の種類テキスト
    std::unique_ptr<PlayerBulletTypeText> playerBulletTypeText_;
    // ダメージ量表示
    std::list<std::unique_ptr<DamageAmountDisplay>> damageAmountDisplays_;
    // ポーズメニュー
    std::unique_ptr<PauseMenu> pauseMenu_;
    // ウェーブ表示
    std::unique_ptr<WaveDisplay> waveDisplay_;
    // レベルアップ表示
    std::unique_ptr<PlayerLevelUpText> playerLevelUpText_;
    // 発射クールダウンゲージ
    std::unique_ptr<PlayerShootCoolDownBar> playerShootCoolDownBar_;
    
    // プレイヤーのキー設定
    KashipanEngine::KeyConfig playerKeyConfig_;
    // UI操作用のキーコンフィグ
    KashipanEngine::KeyConfig uiKeyConfig_;

    // 当たり判定管理
    std::unique_ptr<CollisionManager> collisionManager_;
    // ウェーブ管理
    std::unique_ptr<WaveManager> waveManager_;
    // ウェーブ開始アニメーション
    std::unique_ptr<WaveDrawAnimation> waveDrawAnimation_;
    // リザルト表示アニメーション
    std::unique_ptr<ResultDrawAnimation> resultDrawAnimation_;

    //==================================================
    // シーン切り替えアニメーション
    //==================================================

    // 切り替えるシーン
    std::string nextSceneName_;
    // シーン開始アニメーション
    std::unique_ptr<OpenRectToCenterY> openRectToCenterY_;
    // シーン終了アニメーション
    std::unique_ptr<CloseRectToCenterY> closeRectToCenterY_;
    // ゲームオーバー時のフェードアウト
    std::unique_ptr<FadeOut> fadeOut_;
};