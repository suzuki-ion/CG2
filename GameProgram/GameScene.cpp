#include "GameScene.h"
#include "Base/Input.h"
#include "Base/Renderer.h"

using namespace KashipanEngine;

// 要素数
extern const uint32_t kNumBlockVertical = 20;
extern const uint32_t kNumBlockHorizontal = 100;
// ブロック1個分の横幅
extern const float kBlockWidth = 2.0f;
// ブロック1個分の縦幅
extern const float kBlockHeight = 2.0f;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// レンダラーへのポインタ
Renderer *sRenderer = nullptr;
} // namespace

void GameScene::Initialize(Engine *kashipanEngine) {
    // エンジンへのポインタを保存
    sKashipanEngine = kashipanEngine;
    // レンダラーへのポインタを取得
    sRenderer = kashipanEngine->GetRenderer();
    // カメラの生成
    camera_ = std::make_unique<Camera>();
    // レンダラーにカメラをセット
    sRenderer->SetCamera(camera_.get());

	// プレイヤーの3Dモデルデータの生成
    modelPlayer_ = std::make_unique<Model>("Resources/Player", "player.obj");
    modelPlayer_->SetRenderer(sRenderer);
    // プレイヤーの攻撃用3Dモデルデータの生成
    modelPlayerAttack_ = std::make_unique<Model>("Resources/Attack", "attack.obj");
    modelPlayerAttack_->SetRenderer(sRenderer);
	// 敵キャラの3Dモデルデータの生成
    modelEnemy_ = std::make_unique<Model>("Resources/Enemy", "enemy.obj");
    modelEnemy_->SetRenderer(sRenderer);
	// ブロックの3Dモデルデータの生成
    modelBlock_ = std::make_unique<Model>("Resources/Block", "block.obj");
    modelBlock_->SetRenderer(sRenderer);
	// スカイドームの3Dモデルデータの生成
    modelSkydome_ = std::make_unique<Model>("Resources/Skydome", "skydome.obj");
    modelSkydome_->SetRenderer(sRenderer);
	// 死亡パーティクルの3Dモデルデータの生成
    modelDeathParticle_ = std::make_unique<Model>("Resources/DeathParticle", "deathParticle.obj");
    modelDeathParticle_->SetRenderer(sRenderer);

	// マップチップフィールドの生成
    mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 自キャラの生成
    player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelPlayerAttack_.get(), mapChipField_->GetMapChipPosition(1, kNumBlockVertical - 2));
	player_->SetMapChipField(mapChipField_.get());

	// 敵キャラの生成(3体)
	for (uint32_t i = 0; i < 3; ++i) {
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
		enemy->Initialize(modelEnemy_.get(), mapChipField_->GetMapChipPosition(12 + i, kNumBlockVertical - 2));
		enemy->SetMapChipField(mapChipField_.get());
        enemies_.push_back(std::move(enemy));
	}

	// ブロックの生成
    block_ = std::make_unique<Block>();
	block_->Initialize(modelBlock_.get());

	// スカイドームの生成
    skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

    // カメラコントローラーの生成
    cameraController_ = std::make_unique<CameraController>();
	cameraController_->SetCamera(camera_.get());
	cameraController_->SetTarget(player_.get());

	// 死亡パーティクルの生成
    deathParticle_ = std::make_unique<DeathParticle>();
	deathParticle_->Initialize(modelDeathParticle_.get());

	// フェードの生成
    fade_ = std::make_unique<Fade>();
	fade_->Initialize(sKashipanEngine);

	// フェードインの開始
	fade_->Start(Fade::Status::kFadeIn);

	// マップチップフィールドのブロックを生成
	block_->SetMapChipFieldBlocks(mapChipField_.get());
}

void GameScene::Finalize() {
	// ブロックの解放
	block_->Finalize();
}

void GameScene::Update() {
	// フェーズごとの処理
	switch (phase_) {
	case Phase::kPlay:
		//==================================================
		// ゲームプレイフェーズの更新処理
		//==================================================

		// 自キャラの更新
		player_->Update();

		// 敵キャラの更新
		for (auto& enemy : enemies_) {
			enemy->Update();
		}

		// 総当たり判定
		CheckAllCollisions();

		// ブロックの更新
		block_->Update();

		// カメラコントローラーの更新
		cameraController_->Update();

		break;

	case Phase::kDeath:
		//==================================================
		// デス演出フェーズの更新処理
		//==================================================

		// 死亡パーティクルの更新
		deathParticle_->Update();

		// 敵キャラの更新
		for (auto& enemy : enemies_) {
			enemy->Update();
		}

		// ブロックの更新
		block_->Update();

		// カメラコントローラーの更新
		cameraController_->Update();

		break;
	}

#ifdef _DEBUG
	if (Input::IsKeyTrigger(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		sRenderer->ToggleDebugCamera();
	}

	// F5キーでマップチップフィールドをホットリロード
	if (Input::IsKeyTrigger(DIK_F5)) {
		mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
		block_->SetMapChipFieldBlocks(mapChipField_.get());
	}

	// Rキーでプレイヤーの位置をリセット
	if (Input::IsKeyTrigger(DIK_R)) {
		player_->SetPosition(mapChipField_->GetMapChipPosition(1, kNumBlockVertical - 2));
	}

#endif // _DEBUG

	// フェードの更新
	fade_->Update();

	// フェーズ切り替え処理
	ChangePhase();
}

void GameScene::Draw() {
	// 敵キャラの描画
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}

	// ブロックの描画
	block_->Draw();

	// スカイドームの描画
	skydome_->Draw();

	switch (phase_) {
	case Phase::kPlay:
		//==================================================
		// ゲームプレイフェーズの描画処理
		//==================================================

		// 自キャラの描画
		player_->Draw();

		break;

	case Phase::kDeath:
		//==================================================
		// デス演出フェーズの描画処理
		//==================================================

		// 死亡パーティクルの描画
		deathParticle_->Draw();

		break;
	}

	// フェードの描画
	fade_->Draw();
}

void GameScene::CheckAllCollisions() {
	// プレイヤーと敵キャラの当たり判定
	for (auto& enemy : enemies_) {
		if (player_->GetAABB().IsCollision(enemy->GetAABB())) {
			// 衝突処理
			player_->OnCollision(enemy.get());
			enemy->OnCollision(player_.get());
		}
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			// フェードインが終了したらゲームプレイフェーズに切り替え
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		if (player_->IsDead()) {
			// プレイヤーが死亡したらデスフェーズに切り替え
			phase_ = Phase::kDeath;
			// 死亡パーティクルを出す
			deathParticle_->Emit(player_->GetPosition());
		}
		break;

	case Phase::kDeath:
		if (deathParticle_->IsFinished()) {
			// デスフェーズが終了したらフェードアウトフェーズに切り替え
			phase_ = Phase::kFadeOut;
			// フェードアウトを開始
			fade_->Start(Fade::Status::kFadeOut);
		}
		break;

	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			// フェードアウトが終了したらゲームシーン終了
			isFinished_ = true;
		}
	}
}