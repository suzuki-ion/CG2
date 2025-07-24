#pragma once
#include "Object.h"
#include "Font/FontLoader.h"

namespace KashipanEngine {

enum class TextType {
    Unique, ///< 文字ごとにオブジェクトを生成する
    Shared, ///< 文字列をひとつのオブジェクトとして生成する
};

class Text : public Object {
public:
    Text() = delete;
    /// @brief テキストのコンストラクタ
    /// @param textCount テキストの文字数
    Text(uint32_t textCount);

    /// @brief テキストのフォントの設定
    /// @param fontFilePath フォントファイル(.fnt)のパス
    void SetFont(const char *fontFilePath);

    /// @brief テキストの設定(UTF8文字列)
    /// @param text テキストの文字列(""の前にu8と付けたUTF8形式の文字列。例: u8"Hello, World!")
    void SetText(const std::u8string &text);

    /// @brief テキストの文字列を取得する
    /// @return テキストの文字列
    const std::u8string &GetText() const { return text_; }

    /// @brief テキストの文字数を取得する
    /// @return テキストの文字数
    int GetTextCount() const { return static_cast<int>(text_.size()); }

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

private:
    FontData fontData_;
    std::u8string text_;
    std::vector<uint32_t> fontTextureIDs_;
};

} // namespace KashipanEngine