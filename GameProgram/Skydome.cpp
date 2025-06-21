#include "Skydome.h"
#include <cassert>

using namespace KashipanEngine;

void Skydome::Initialize(KashipanEngine::Model *model) {
	// NULLポインタチェック
	assert(model);
	// モデルの設定
	model_ = model;
    // ワールド変換データの生成
    worldTransform_ = std::make_unique<WorldTransform>();
}

void Skydome::Update() {
	worldTransform_->TransferMatrix();
}

void Skydome::Draw() {
	model_->Draw(*worldTransform_);
}