#pragma once
#include <string>
#include <array>
#include <unordered_map>

namespace KashipanEngine {

enum class PaddingDirection {
    Top,    ///< 上のパディング
    Right,  ///< 右のパディング
    Bottom, ///< 下のパディング
    Left    ///< 左のパディング
};

enum class SpacingDirection {
    X,      ///< X方向の文字間隔
    Y       ///< Y方向の文字間隔
};

/// @brief フォント全体の基本情報
struct FontInfo {
    std::string face;   ///< フォント名
    std::string charset;///< 使用する文字セット
    float stretchH;     ///< 文字の縦伸縮率
    int size;           ///< フォントサイズ
    int aa;             ///< アンチエイリアスレベル (0: 無効, 1: 有効)
    int padding[4];     ///< 文字のパディング (上, 右, 下, 左)
    int spacing[2];     ///< 文字間隔 (X, Y)
    int outline;        ///< アウトラインの太さ(px)
    bool isBold;        ///< 太字フラグ
    bool isItalic;      ///< 斜体フラグ
    bool isUnicode;     ///< Unicode使用フラグ
    bool isSmooth;      ///< アンチエイリアスフラグ
};

/// @brief 描画に必要な共通設定
struct FontCommon {
    float lineHeight;   ///< 行の高さ (改行時のピクセル数)
    float base;         ///< ベースラインの位置 (文字の基準線)
    float scaleW;       ///< フォントテクスチャの幅
    float scaleH;       ///< フォントテクスチャの高さ
    int pages;          ///< 使用するページ数 (テクスチャの数)
    int alphaChannel;   ///< アルファチャンネルの使用状況
    int redChannel;     ///< 赤チャンネルの使用状況
    int greenChannel;   ///< 緑チャンネルの使用状況
    int blueChannel;    ///< 青チャンネルの使用状況
    bool isPacked;      ///< テクスチャが圧縮されているか
};

/// @brief フォント画像(テクスチャ)の情報
struct FontPage {
    int id;             ///< ページID (0から始まる)
    int textureIndex;   ///< 使用するテクスチャのインデックス
    std::string file;   ///< ページに対応する画像ファイル名
};

/// @brief 文字ごとの情報
struct CharInfo {
    int id;             ///< 文字ID(Unicode)
    int page;           ///< 文字が属するページ番号
    int channel;        ///< 文字が属するチャンネル番号
    float x;            ///< 画像上の左上X座標
    float y;            ///< 画像上の左上Y座標
    float width;        ///< 文字の幅
    float height;       ///< 文字の高さ
    float xOffset;      ///< 描画位置のXオフセット
    float yOffset;      ///< 描画位置のYオフセット
    float xAdvance;     ///< 次の文字の描画位置
};

/// @brief フォント全体のデータ構造
struct FontData {
    // フォントの基本情報
    FontInfo info;
    // 描画に必要な共通設定
    FontCommon common;
    // 使用するフォントページの情報
    std::vector<FontPage> pages;
    // 文字ごとの情報 (文字IDをキーとするマップ)
    std::unordered_map<int, CharInfo> chars;
    // 文字の数
    int charsCount;
};

} // namespace KashipanEngine