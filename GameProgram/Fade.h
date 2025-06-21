#pragma once
#include "KashipanEngine.h"
#include "Objects.h"

class Fade {
public:
	enum class Status {
		kNone,		// フェードなし
		kFadeIn,    // フェードイン
		kFadeOut,	// フェードアウト
	};

	// フェードの初期化
    void Initialize(Engine *kashipanEngine);

	// フェード開始
	void Start(Status status, float duration = 1.0f);
	// フェード停止
	void Stop();

	// フェードの更新
	void Update();
	// フェードの描画
	void Draw();

	// フェード終了判定
	bool IsFinished() const;

private:
	// テクスチャハンドル
	int textureHandle_ = -1;
	// スプライト
	std::unique_ptr<KashipanEngine::Sprite> sprite_;
	// スプライトの情報
    KashipanEngine::Object::StatePtr spriteState_;

	// フェードの状態
	Status status_ = Status::kNone;

	// フェードの時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};
