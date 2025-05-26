#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <d3dx12.h>
#include <wrl.h>
#include <DirectXTex.h>

#include "Common/Texture.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;

/// @brief テクスチャ管理クラス
class TextureManager {
public:
    TextureManager(WinApp *winApp, DirectXCommon *dxCommon);
    ~TextureManager();

    /// @brief テクスチャの読み込み
    /// @param filePath 読み込むテクスチャのファイル名
    /// @return テクスチャを読み込んだインデックス
    uint32_t Load(const std::string &filePath);

    /// @brief テクスチャデータの取得
    /// @param index テクスチャのインデックス
    /// @return テクスチャデータ
    [[nodiscard]] const Texture &GetTexture(uint32_t index) const;

private:
    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;

    /// @brief テクスチャのデータ
    std::vector<Texture> textures_;

    /// @brief テクスチャのリソースを作成
    /// @param metadata テクスチャのメタデータ
    void CreateTextureResource(const DirectX::TexMetadata &metadata);

    /// @brief TextureResourceにデータをアップロードする
    /// @param texture テクスチャリソース
    /// @param mipImages ミップマップ画像
    /// @return アップロードしたリソース
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages);
};

} // namespace KashipanEngine