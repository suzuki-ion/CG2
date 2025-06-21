#include "Block.h"
#include "MapChipField.h"
#include <cassert>

using namespace KashipanEngine;

extern const uint32_t kNumBlockVertical;
extern const uint32_t kNumBlockHorizontal;
extern const float kBlockWidth;
extern const float kBlockHeight;

void Block::Initialize(KashipanEngine::Model *model) {
    // NULLポインタチェック
    assert(model);
    // モデルの設定
    model_ = model;
    // 要素数を変更する
    worldTransforms_.resize(kNumBlockVertical);
    for (uint32_t i = 0; i < kNumBlockVertical; i++) {
        worldTransforms_[i].resize(kNumBlockHorizontal);
    }
}

void Block::Finalize() {
    // ブロック用のワールドトランスフォームの解放
    ResetWorldTransforms();
}

void Block::SetMapChipFieldBlocks(MapChipField *mapChipField) {
    // NULLポインタチェック
    assert(mapChipField);

    // 一度ワールドトランスフォームを解放
    ResetWorldTransforms();
    mapChipField_ = mapChipField;
    // ワールドトランスフォームの生成
    CreateWorldTransforms();
}

void Block::Update() {
    for (auto &worldTransformBlockLine : worldTransforms_) {
        for (auto &worldTransformBlock : worldTransformBlockLine) {
            if (!worldTransformBlock) {
                continue;
            }
            // ワールドトランスフォームの更新
            worldTransformBlock->TransferMatrix();
        }
    }
}

void Block::Draw() {
    // 3Dモデルを描画
    for (auto &worldTransformBlockLine : worldTransforms_) {
        for (auto &worldTransformBlock : worldTransformBlockLine) {
            if (!worldTransformBlock) {
                continue;
            }
            model_->Draw(*worldTransformBlock);
        }
    }
}

void Block::ResetWorldTransforms() {
    for (auto &worldTransformBlockLine : worldTransforms_) {
        for (auto &worldTransformBlock : worldTransformBlockLine) {
            worldTransformBlock.reset();
        }
    }
    worldTransforms_.clear();
}

void Block::CreateWorldTransforms() {
    // 要素数を変更する
    worldTransforms_.resize(kNumBlockVertical);
    for (uint32_t i = 0; i < kNumBlockVertical; i++) {
        worldTransforms_[i].resize(kNumBlockHorizontal);
    }
    // ブロックの生成
    for (uint32_t i = 0; i < kNumBlockVertical; i++) {
        for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
            if (mapChipField_->GetMapChipType(j, i) == MapChipType::kBlank) {
                continue;
            }
            worldTransforms_[i][j] = std::make_unique<WorldTransform>();
            worldTransforms_[i][j]->translate_ = mapChipField_->GetMapChipPosition(j, i);
        }
    }
}
