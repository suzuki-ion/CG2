#include <utf8.h>
#include <filesystem>
#include "Base/Texture.h"
#include "Text.h"

namespace KashipanEngine {

Text::Text(uint32_t textCount) {
    textCodePoints_.resize(textCount, -1);
    vertexCount_ = textCount * 4;
    indexCount_ = textCount * 6;
    Create(vertexCount_, indexCount_);
    for (UINT i = 0; i < vertexCount_; ++i) {
        mesh_->vertexBufferMap[i].position = { 0.0f, 0.0f, 0.0f, 1.0f };
        mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        mesh_->vertexBufferMap[i].texCoord = { 0.0f, 0.0f };
    }

    for (uint32_t i = 0; i < textCount; ++i) {
        uint32_t vertexIndex = i * 4;
        uint32_t indexIndex = i * 6;
        mesh_->indexBufferMap[indexIndex + 0] = vertexIndex + 0;
        mesh_->indexBufferMap[indexIndex + 1] = vertexIndex + 1;
        mesh_->indexBufferMap[indexIndex + 2] = vertexIndex + 2;
        mesh_->indexBufferMap[indexIndex + 3] = vertexIndex + 1;
        mesh_->indexBufferMap[indexIndex + 4] = vertexIndex + 3;
        mesh_->indexBufferMap[indexIndex + 5] = vertexIndex + 2;
    }

    isUseCamera_ = false;
    material_.lightingType = 0;
}

void Text::SetFont(const char *fontFilePath) {
    // 後ほど画像を読み込む際に使うためディレクトリパスの取得をする
    std::string directory = std::filesystem::path(fontFilePath).parent_path().string();

    // .fnt読み込み
    fontData_ = LoadFNT(fontFilePath);
    
    // 使用するテクスチャを取得
    for (auto &page : fontData_.pages) {
        // 最初に取得したディレクトリパスとファイル名を結合してフルパスを作成
        page.textureIndex = Texture::Load(directory + '/' + page.file);
    }

    // まだ1メッシュに対して複数テクスチャ使えないので、最初のテクスチャを使う
    useTextureIndex_ = static_cast<int>(fontData_.pages[0].textureIndex);
}

void Text::SetFont(const FontData &fontData) {
    fontData_ = fontData;
    // まだ1メッシュに対して複数テクスチャ使えないので、最初のテクスチャを使う
    useTextureIndex_ = static_cast<int>(fontData_.pages[0].textureIndex);
}

void Text::SetText(const std::u8string &text) {
    // 渡された文字列が現在のテキストと同じなら何もしない
    if (text_ == text) {
        return;
    }

    text_ = text;
    lineInfos_.clear();
    lineInfos_.emplace_back(LineInfo());
    lineInfos_.back().beginVertexIndex = 0;
    lineInfos_.back().height = fontData_.common.lineHeight;
    UINT charIndex = 0;
    UINT vertexIndex = 0;
    float cursorX = 0.0f;
    float cursorY = 0.0f;
    auto it = text_.begin();
    auto end = text_.end();
    while (it != end) {
        // 頂点インデックスが頂点数を超えたら強制終了
        if (vertexIndex >= vertexCount_) {
            break;
        }

        // 文字の情報を取得
        auto codePoint = utf8::next(it, end);
        auto &charData = fontData_.chars[codePoint];

        // 文字が改行コードだったらカーソルを改行位置に移動
        if (codePoint == '\n') {
            cursorX = 0.0f;
            cursorY += fontData_.common.lineHeight;
            lineInfos_.back().endVertexIndex = vertexIndex;
            lineInfos_.emplace_back(LineInfo());
            lineInfos_.back().beginVertexIndex = vertexIndex;
            lineInfos_.back().height = fontData_.common.lineHeight;
            continue;
        }

        for (int y = 0; y < 2; ++y) {
            for (int x = 0; x < 2; ++x) {
                // uv座標を計算
                int index = vertexIndex + (y + x * 2);
                float textureX = static_cast<float>(x) * charData.width + charData.x;
                float textureY = static_cast<float>(1 - y) * charData.height + charData.y;
                textureX /= fontData_.common.scaleW;
                textureY /= fontData_.common.scaleH;
                mesh_->vertexBufferMap[index].texCoord.x = textureX;
                mesh_->vertexBufferMap[index].texCoord.y = textureY;

                // 頂点座標を計算
                float posX = cursorX + static_cast<float>(x) * charData.width;
                float posY = cursorY + static_cast<float>(1 - y) * charData.height;
                posX += charData.xOffset;
                posY += charData.yOffset;
                mesh_->vertexBufferMap[index].position.x = posX;
                mesh_->vertexBufferMap[index].position.y = posY;
            }
        }

        // インデックスを更新
        vertexIndex += 4;
        // カーソルを更新
        cursorX += charData.xAdvance;
        // テキストの幅を更新
        lineInfos_.back().width += charData.xAdvance;
        // 文字コードを保存
        if (charIndex < textCodePoints_.size()) {
            textCodePoints_[charIndex] = codePoint;
        }
        // 文字インデックスを更新
        charIndex++;
        
    }
    // 最後の行の頂点インデックスを設定
    lineInfos_.back().endVertexIndex = vertexIndex;

    // テキストの位置を再計算
    TextAlignX currentTextAlignX = textAlignX_;
    TextAlignY currentTextAlignY = textAlignY_;
    textAlignX_ = TextAlignX::Left;
    textAlignY_ = TextAlignY::Top;
    SetTextAlign(currentTextAlignX, currentTextAlignY);

    // 使用していない文字コードをクリア
    for (UINT i = charIndex; i < textCodePoints_.size(); ++i) {
        textCodePoints_[i] = -1;
    }
    // 残りの頂点をクリア
    for (UINT i = vertexIndex; i < vertexCount_; ++i) {
        mesh_->vertexBufferMap[i].position = { 0.0f, 0.0f, 0.0f, 1.0f };
        mesh_->vertexBufferMap[i].texCoord = { 0.0f, 0.0f };
    }
}

void Text::SetTextAlign(TextAlignX textAlignX, TextAlignY textAlignY) {
    if (textAlignX_ != textAlignX) {
        CalculateTextAlignX(textAlignX);
        textAlignX_ = textAlignX;
    }
    if (textAlignY_ != textAlignY) {
        CalculateTextAlignY(textAlignY);
        textAlignY_ = textAlignY;
    }
}

void Text::CalculateTextAlignX(TextAlignX newTextAlignX) {
    // 一度元の位置に戻す
    for (auto &lineInfo : lineInfos_) {
        // テキストの幅を取得
        float textWidth = lineInfo.width;
        // テキストのX座標を計算
        float offsetX = 0.0f;
        if (textAlignX_ == TextAlignX::Center) {
            offsetX = textWidth / 2.0f;
        } else if (textAlignX_ == TextAlignX::Right) {
            offsetX = textWidth;
        }
        
        // 各頂点のX座標を更新
        for (UINT i = lineInfo.beginVertexIndex; i < lineInfo.endVertexIndex; ++i) {
            mesh_->vertexBufferMap[i].position.x += offsetX;
        }
    }

    // 新しい位置に移動
    for (auto &lineInfo : lineInfos_) {
        // テキストの幅を取得
        float textWidth = lineInfo.width;
        // テキストのX座標を計算
        float offsetX = 0.0f;
        if (newTextAlignX == TextAlignX::Center) {
            offsetX = -textWidth / 2.0f;
        } else if (newTextAlignX == TextAlignX::Right) {
            offsetX = -textWidth;
        }
        
        // 各頂点のX座標を更新
        for (UINT i = lineInfo.beginVertexIndex; i < lineInfo.endVertexIndex; ++i) {
            mesh_->vertexBufferMap[i].position.x += offsetX;
        }
    }
}

void Text::CalculateTextAlignY(TextAlignY newTextAlignY) {
    // 一度元の位置に戻す
    for (auto &lineInfo : lineInfos_) {
        // テキストの高さを取得
        float textHeight = lineInfo.height;
        // テキストのY座標を計算
        float offsetY = 0.0f;
        if (textAlignY_ == TextAlignY::Center) {
            offsetY = textHeight / 2.0f;
        } else if (textAlignY_ == TextAlignY::Bottom) {
            offsetY = textHeight;
        }
        
        // 各頂点のY座標を更新
        for (UINT i = lineInfo.beginVertexIndex; i < lineInfo.endVertexIndex; ++i) {
            mesh_->vertexBufferMap[i].position.y += offsetY;
        }
    }

    // 新しい位置に移動
    for (auto &lineInfo : lineInfos_) {
        // テキストの高さを取得
        float textHeight = lineInfo.height;
        // テキストのY座標を計算
        float offsetY = 0.0f;
        if (newTextAlignY == TextAlignY::Center) {
            offsetY = -textHeight / 2.0f;
        } else if (newTextAlignY == TextAlignY::Bottom) {
            offsetY = -textHeight;
        }
        
        // 各頂点のY座標を更新
        for (UINT i = lineInfo.beginVertexIndex; i < lineInfo.endVertexIndex; ++i) {
            mesh_->vertexBufferMap[i].position.y += offsetY;
        }
    }
}

} // namespace KashipanEngine