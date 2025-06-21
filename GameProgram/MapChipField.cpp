#include "MapChipField.h"
#include <cassert>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

extern const uint32_t kNumBlockVertical;
extern const uint32_t kNumBlockHorizontal;
extern const float kBlockWidth;
extern const float kBlockHeight;

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

} // namespace

void MapChipField::ResetMapChipData() {
	// マップチップデータをリセット
	mapChipData_.data_.clear();
	mapChipData_.data_.resize(kNumBlockVertical);
	for (auto& mapChipBlockLine : mapChipData_.data_) {
		mapChipBlockLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCSV;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCSV << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		std::string line;
		std::getline(mapChipCSV, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::stringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data_[i][j] = mapChipTable[word];
			} else {
				// 念のため未知の値は空白にする
				mapChipData_.data_[i][j] = MapChipType::kBlank;
			}
		}
	}
}

MapChipType MapChipField::GetMapChipType(uint32_t x, uint32_t y) {
	if (x < 0 || x >= kNumBlockHorizontal || y < 0 || y >= kNumBlockVertical) {
		// 範囲外の場合は空白を返す
		return MapChipType::kBlank;
	}

	return mapChipData_.data_[y][x];
}

KashipanEngine::Vector3 MapChipField::GetMapChipPosition(uint32_t x, uint32_t y) {
	if (x < 0 || x >= kNumBlockHorizontal || y < 0 || y >= kNumBlockVertical) {
		// 範囲外の場合は空白を返す
		return KashipanEngine::Vector3(0, 0, 0);
	}

	return KashipanEngine::Vector3(kBlockWidth * x, kBlockHeight * (kNumBlockVertical - 1 - y), 0);
}

IndexSet MapChipField::GetMapChipIndex(const KashipanEngine::Vector3& position) {
	IndexSet indexSet{};
	// x座標からインデックスを計算
	indexSet.x = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	// y座標からインデックスを計算
	indexSet.y = kNumBlockVertical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight);
	return indexSet;
}

Rect MapChipField::GetRect(uint32_t x, uint32_t y) {
	KashipanEngine::Vector3 center = GetMapChipPosition(x, y);
	Rect rect{};
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

