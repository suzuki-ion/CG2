#include <utf8.h>
#include <filesystem>
#include "Base/Texture.h"
#include "Text.h"

namespace KashipanEngine {

Text::Text(uint32_t textCount) {
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
}

void Text::SetFont(const char *fontFilePath) {
    // 後ほど画像を読み込む際に使うためディレクトリパスの取得をする
    std::string directory = std::filesystem::path(fontFilePath).parent_path().string();

    // .fnt読み込み
    fontData_ = LoadFNT(fontFilePath);
    
    // 使用するテクスチャを取得
    fontTextureIDs_.clear();
    fontTextureIDs_.resize(fontData_.pages.size());
    for (auto &page : fontData_.pages) {
        // 最初に取得したディレクトリパスとファイル名を結合してフルパスを作成
        fontTextureIDs_[page.id] = Texture::Load(directory + '/' + page.file);
    }

    // まだ1メッシュに対して複数テクスチャ使えないので、最初のテクスチャを使う
    useTextureIndex_ = static_cast<int>(fontTextureIDs_[0]);
}

void Text::SetText(const std::u8string &text) {
    // 渡された文字列が現在のテキストと同じなら何もしない
    if (text_ == text) {
        return;
    }

    text_ = text;

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
    }

    // 残りの頂点をクリア
    for (UINT i = vertexIndex; i < vertexCount_; ++i) {
        mesh_->vertexBufferMap[i].position = { 0.0f, 0.0f, 0.0f, 1.0f };
        mesh_->vertexBufferMap[i].texCoord = { 0.0f, 0.0f };
    }
}

void Text::Draw() {
    DrawCommon();
}

void Text::Draw(WorldTransform &worldTransform) {
    DrawCommon(worldTransform);
}

} // namespace KashipanEngine