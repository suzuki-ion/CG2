#include "TitleScene.h"
#include "Base/Renderer.h"
#include "Base/Input.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// レンダラーへのポインタ
Renderer *sRenderer = nullptr;
} // namespace

void TitleScene::Initialize(Engine *kashipanEngine) {
    // エンジンのポインタを保存
    sKashipanEngine = kashipanEngine;
    // レンダラーのポインタを取得
    sRenderer = kashipanEngine->GetRenderer();

	// カメラの生成
	camera_ = std::make_unique<Camera>();
    camera_->GetTranslatePtr()->z = -16.0f;
    // レンダラーにカメラをセット
    sRenderer->SetCamera(camera_.get());

    // ライトの初期化
    directionalLight_.direction = { 0.5f, -0.75f, -0.5f };
    directionalLight_.color = { 255.0f, 255.0f, 255.0f, 255.0f };
	directionalLight_.intensity = 1.0f;

	// タイトル文字の3Dモデルデータの生成
    modelTitleText_ = std::make_unique<Model>("Resources/TitleText", "titleText.obj");
    modelTitleText_->SetRenderer(sRenderer);

	// ワールド変換データの生成
    worldTransformTitleText_ = std::make_unique<WorldTransform>();

	// フェードの生成と初期化
    fade_ = std::make_unique<Fade>();
	fade_->Initialize(sKashipanEngine);

	// フェードの開始
	fade_->Start(Fade::Status::kFadeIn);
}

void TitleScene::Update() {
	// タイトル文字の移動速度
	static const float kMoveSpeed = 0.04f;
	// アニメーション用の角度
	static float angle = 0.0f;

	switch (phase_) {
	case Phase::kMain:
		// スペースキーが押されたらフェードを開始
		if (Input::IsKeyDown(DIK_SPACE)) {
			fade_->Start(Fade::Status::kFadeOut);
		}
		break;
	}

	// 角度を更新
	angle += kMoveSpeed;
	// タイトル文字の位置を更新
	worldTransformTitleText_->translate_.y = std::sin(angle);

	// フェードの更新
	fade_->Update();

	// フェーズの切り替え
	ChangePhase();
}

void TitleScene::Draw() {
	// レンダラーにライトをセット
	sRenderer->SetLight(&directionalLight_);

	// 3Dモデルを描画
	modelTitleText_->Draw(*worldTransformTitleText_);

	// フェードの描画
	fade_->Draw();
}

void TitleScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		// フェードインが終了したらメインフェーズに移行
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// フェードが始まったらフェードアウトフェーズに移行
		if (!fade_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		// フェードアウトが終了したら終了フラグを立てる
		if (fade_->IsFinished()) {
			isFinalized_ = true;
		}
		break;
	}
}
