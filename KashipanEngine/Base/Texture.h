#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <d3dx12.h>
#include <wrl.h>
#include <DirectXTex.h>

#include "Common/TextureData.h"

namespace KashipanEngine {

// 前方宣言
class DirectXCommon;

/// @brief テクスチャ管理クラス
class Texture {
public:
    /// @brief テクスチャ管理の初期化
    /// @param dxCommon DirectXCommonインスタンス
    static void Initialize(DirectXCommon *dxCommon);

    /// @brief テクスチャ管理の終了処理
    static void Finalize();

    /// @brief テクスチャの読み込み
    /// @param filePath 読み込むテクスチャのファイル名
    /// @return テクスチャを読み込んだインデックス
    static uint32_t Load(const std::string &filePath);

    /// @brief テクスチャデータの取得
    /// @param index テクスチャのインデックス
    /// @return テクスチャデータ
    static [[nodiscard]] const TextureData &GetTexture(uint32_t index);

    /// @brief テクスチャデータの取得
    /// @param filePath テクスチャのファイルパス
    /// @return テクスチャデータ
    static [[nodiscard]] const TextureData &GetTexture(const std::string &filePath);
};

} // namespace KashipanEngine