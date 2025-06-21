#pragma once
#include "KashipanEngine.h"
#include "Objects.h"
#include "3d/DirectionalLight.h"
#include "Math/Camera.h"
#include "Fade.h"

class TitleScene {
public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,	// フェードイン
		kMain,		// メイン部
		kFadeOut,	// フェードアウト
	};

	// 初期化
	void Initialize(Engine *kashipanEngine);
	// 更新
	void Update();
	// 描画
	void Draw();

	// 終了フラグの取得
	bool IsFinalized() const { return isFinalized_; }

private:
	// フェーズ切り替え処理
	void ChangePhase();

	// タイトル文字の3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelTitleText_;
	// ワールド変換データ
	std::unique_ptr<KashipanEngine::WorldTransform> worldTransformTitleText_;
	// カメラ
	std::unique_ptr<KashipanEngine::Camera> camera_;
    // ライト
    KashipanEngine::DirectionalLight directionalLight_;

	// フェード処理
	std::unique_ptr<Fade> fade_;

	// 終了フラグ
	bool isFinalized_ = false;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};
