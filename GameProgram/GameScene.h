#pragma once
#include "KashipanEngine.h"
#include "Objects.h"
#include "Math/Camera.h"

#include "Player.h"
#include "Enemy.h"
#include "Block.h"
#include "Skydome.h"
#include "CameraController.h"
#include "MapChipField.h"
#include "DeathParticle.h"
#include "Fade.h"
#include <vector>
#include <list>

class GameScene {
public:
	// 初期化
	void Initialize(Engine *kashipanEngine);

	// 終了処理
	void Finalize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了フラグの取得
	bool IsFinished() const { return isFinished_; }

private:
	enum class Phase {
		kFadeIn,	// フェードイン
		kPlay,		// ゲームプレイ
		kDeath,		// デス演出
		kFadeOut,	// フェードアウト
	};

	// 全ての総当たり判定を行う
	void CheckAllCollisions();
	// フェーズ切り替え処理
	void ChangePhase();

	// ゲームの現在フェーズ
	Phase phase_ = Phase::kPlay;

	// プレイヤーの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelPlayer_ = nullptr;
    // プレイヤーの攻撃用3Dモデルデータ
    std::unique_ptr<KashipanEngine::Model> modelPlayerAttack_ = nullptr;
	// 敵キャラの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelEnemy_ = nullptr;
	// ブロックの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelBlock_ = nullptr;
	// スカイドームの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelSkydome_ = nullptr;
	// 死亡パーティクルの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelDeathParticle_ = nullptr;
	// カメラコントローラー
	std::unique_ptr<CameraController> cameraController_;
	// マップチップフィールド
	std::unique_ptr<MapChipField> mapChipField_;

	// カメラ
	std::unique_ptr<KashipanEngine::Camera> camera_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// 自キャラ
	std::unique_ptr<Player> player_;
	// 敵キャラ
	std::list<std::unique_ptr<Enemy>> enemies_;
	// ブロック
	std::unique_ptr<Block> block_;
	// スカイドーム
	std::unique_ptr<Skydome> skydome_;
	// 死亡パーティクル
	std::unique_ptr<DeathParticle> deathParticle_;
	// フェード
	std::unique_ptr<Fade> fade_;

	// 終了フラグ
	bool isFinished_ = false;
};
