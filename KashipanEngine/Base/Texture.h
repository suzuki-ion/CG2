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
    /// @param textureName テクスチャの名前(設定しなかった場合はファイルパスになる)
    /// @return テクスチャを読み込んだインデックス
    static uint32_t Load(const std::string &filePath, const std::string &textureName = "");

    /// @brief Jsonファイルからのテクスチャの一括読み込み
    /// @param jsonFilePath Jsonファイルのパス
    static void LoadFromJson(const std::string &jsonFilePath);

    /// @brief テクスチャ管理クラスが管理するテクスチャの追加
    /// @param textureData 追加するテクスチャデータ
    /// @return 追加したテクスチャのインデックス
    static uint32_t AddData(const TextureData &textureData);

    /// @brief テクスチャデータの変更
    /// @param textureData 変更するテクスチャデータ
    /// @param index 変更するテクスチャのインデックス
    static void ChangeData(const TextureData &textureData, uint32_t index);

    /// @brief テクスチャのインデックスの取得
    /// @param textureName テクスチャの名前
    /// @return テクスチャのインデックス。 見つからなかった場合は0を返す
    static int32_t FindIndex(const std::string &textureName);

    /// @brief テクスチャデータの取得
    /// @param index テクスチャのインデックス
    /// @return テクスチャデータ
    static [[nodiscard]] const TextureData &GetTexture(uint32_t index);

    /// @brief テクスチャデータの取得
    /// @param textureName テクスチャの名前
    /// @return テクスチャデータ
    static [[nodiscard]] const TextureData &GetTexture(const std::string &textureName);
};

} // namespace KashipanEngine