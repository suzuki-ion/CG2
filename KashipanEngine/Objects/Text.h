#pragma once
#include "Object.h"
#include "Font/FontLoader.h"

namespace KashipanEngine {

enum class TextType {
    Unique, ///< 文字ごとにオブジェクトを生成する
    Shared, ///< 文字列をひとつのオブジェクトとして生成する
};

enum class TextAlignX {
    Left,   ///< 左揃え
    Center, ///< 中央揃え
    Right,  ///< 右揃え
};

enum class TextAlignY {
    Top,    ///< 上揃え
    Center, ///< 中央揃え
    Bottom, ///< 下揃え
};

class Text : public Object {
public:
    struct LineInfo {
        // 行の幅
        float width = 0.0f;
        // 行の高さ
        float height = 0.0f;
        // 行の最初の頂点インデックス
        uint32_t beginVertexIndex = 0;
        // 行の最後の頂点インデックス
        uint32_t endVertexIndex = 0;
    };

    Text() = delete;
    /// @brief テキストのコンストラクタ
    /// @param textCount テキストの文字数
    Text(uint32_t textCount);

    /// @brief テキストのフォントの設定
    /// @param fontFilePath フォントファイル(.fnt)のパス
    void SetFont(const char *fontFilePath);

    /// @brief テキストのフォントの設定
    /// @param fontData フォントデータ
    void SetFont(const FontData &fontData);

    /// @brief テキストの設定(UTF8文字列)
    /// @param text テキストの文字列(""の前にu8と付けたUTF8形式の文字列。例: u8"Hello, World!")
    void SetText(const std::u8string &text);

    /// @brief テキストの揃え方を設定する
    /// @param textAlignX 水平方向の揃え方
    /// @param textAlignY 垂直方向の揃え方
    void SetTextAlign(TextAlignX textAlignX, TextAlignY textAlignY);

    /// @brief テキストのフォントデータを取得する
    /// @return テキストのフォントデータ
    const FontData &GetFontData() const { return fontData_; }

    /// @brief テキストの文字列を取得する
    /// @return テキストの文字列
    const std::u8string &GetText() const { return text_; }

    /// @brief テキストの文字数を取得する
    /// @return テキストの文字数
    int GetTextCount() const { return static_cast<int>(text_.size()); }

private:
    /// @brief @brief テキストのX軸方向の揃え方を計算する
    void CalculateTextAlignX(TextAlignX newTextAlignX);
    /// @brief テキストのY軸方向の揃え方を計算する
    void CalculateTextAlignY(TextAlignY newTextAlignY);

    FontData fontData_;
    std::u8string text_;
    std::vector<int> textCodePoints_;
    std::vector<LineInfo> lineInfos_;
    TextType textType_ = TextType::Shared;
    TextAlignX textAlignX_ = TextAlignX::Left;
    TextAlignY textAlignY_ = TextAlignY::Top;
};

} // namespace KashipanEngine