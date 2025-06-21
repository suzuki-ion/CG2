#pragma once
#include "math/Vector3.h"
#include <vector>
#include <string>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data_;
};

struct IndexSet {
	uint32_t x;
	uint32_t y;
};

struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

class MapChipField {
public:
	// マップチップデータのリセット
	void ResetMapChipData();
	// CSVファイルからマップチップデータを読み込む
	void LoadMapChipCsv(const std::string& filePath);

	// マップチップの取得
	MapChipType GetMapChipType(uint32_t x, uint32_t y);

	// マップチップの座標を取得
	KashipanEngine::Vector3 GetMapChipPosition(uint32_t x, uint32_t y);

	// マップチップ番号を取得
	IndexSet GetMapChipIndex(const KashipanEngine::Vector3 &position);

	// ブロックの範囲を取得
	Rect GetRect(uint32_t x, uint32_t y);

private:
	MapChipData mapChipData_;
};
