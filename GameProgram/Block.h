#pragma once
#include "Objects.h"
#include "Math/Camera.h"

// 前方宣言
class MapChipField;

class Block {
public:
	/// @brief 初期化
	/// @param model モデル
	/// @param camera カメラ
	/// @param debugCamera デバッグカメラ
	void Initialize(KashipanEngine::Model* model);
	/// @brief 終了処理
	void Finalize();

	/// @brief マップチップフィールドのブロックを生成
	/// @param mapChipField マップチップフィールド
	void SetMapChipFieldBlocks(MapChipField* mapChipField);

	// 更新
	void Update();
	// 描画
	void Draw();

private:
    // ワールド変換データのリセット
    void ResetWorldTransforms();
    // ワールド変換データの生成
    void CreateWorldTransforms();

	// ワールド変換データ
	std::vector<std::vector<std::unique_ptr<KashipanEngine::WorldTransform>>> worldTransforms_;
	// モデル
	KashipanEngine::Model* model_ = nullptr;
    // マップチップフィールド
    MapChipField *mapChipField_ = nullptr;
};
