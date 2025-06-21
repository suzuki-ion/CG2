#include "Base/WinApp.h"
#include "Base/Texture.h"
#include "Fade.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// レンダラーへのポインタ
Renderer *sRenderer = nullptr;
// WinAppへのポインタ
WinApp *sWinApp = nullptr;
} // namespace

void Fade::Initialize(Engine *kashipanEngine) {
    // エンジンへのポインタを保存
    sKashipanEngine = kashipanEngine;
    // レンダラーへのポインタを取得
    sRenderer = kashipanEngine->GetRenderer();
    // WinAppへのポインタを取得
    sWinApp = kashipanEngine->GetWinApp();

	// テクスチャの読み込み
    textureHandle_ = Texture::Load("Resources/white1x1.png");
    // スプライトの生成
    sprite_ = std::make_unique<Sprite>(textureHandle_);
    sprite_->SetRenderer(sRenderer);
	// スプライト情報の取得
    spriteState_ = sprite_->GetStatePtr();
    spriteState_.material->color = Vector4(0.0f, 0.0f, 0.0f, 255.0f);
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	// フェードを停止
	status_ = Status::kNone;
	counter_ = 0.0f;
}

void Fade::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Status::kNone:
		// フェードなし
		return;

	case Status::kFadeIn:
		spriteState_.transform->scale.x = static_cast<float>(sWinApp->GetClientWidth());
		spriteState_.transform->scale.y = static_cast<float>(sWinApp->GetClientHeight());
		// フェードイン処理
		if (counter_ > duration_) {
			counter_ = duration_;
			status_ = Status::kNone; // フェード完了
		} else {
			counter_ += sKashipanEngine->GetDeltaTime();
		}
		// アルファ値を計算
		spriteState_.material->color = Vector4(0.0f, 0.0f, 0.0f, (1.0f - (counter_ / duration_)) * 255.0f);
		break;

	case Status::kFadeOut:
		spriteState_.transform->scale.x = static_cast<float>(sWinApp->GetClientWidth());
		spriteState_.transform->scale.y = static_cast<float>(sWinApp->GetClientHeight());
		// フェードアウト処理
		if (counter_ > duration_) {
			counter_ = duration_;
			status_ = Status::kNone; // フェード完了
        } else {
            counter_ += sKashipanEngine->GetDeltaTime();
        }
		// アルファ値を計算
		spriteState_.material->color = Vector4(0.0f, 0.0f, 0.0f, (counter_ / duration_) * 255.0f);
		break;
	}
}

void Fade::Draw() {
	// 状態がNoneの場合は描画しない
	if (status_ == Status::kNone) {
		return;
	}

	sprite_->Draw();
}

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::kFadeIn:
	case Status::kFadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}

	return true;
}
