#include "TextureManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"
#include "Common/Logs.h"
#include "Common/ConvertString.h"
#include "Common/Descriptors/SRV.h"

namespace KashipanEngine {

namespace {

/// @brief テクスチャファイルを読み込んで扱えるようにする
/// @param filePath テクスチャファイルのパス
/// @return 読み込んだテクスチャのScratchImage
DirectX::ScratchImage LoadTexture(const std::string &filePath) {
    // テクスチャファイルを読み込んで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        nullptr,
        image
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // ミップマップの作成
    // サイズが1x1のテクスチャはミップマップを作成しない
    if (image.GetMetadata().width == 1 && image.GetMetadata().height == 1) {
        return image;
    }
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        DirectX::TEX_FILTER_SRGB,
        0,
        mipImages
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    
    // ミップマップ付きのデータを返す
    return mipImages;
}

} // namespace

TextureManager::TextureManager(WinApp *winApp, DirectXCommon *dxCommon) {
    // nullチェック
    if (winApp == nullptr) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }
    // 引数をメンバ変数に格納
    winApp_ = winApp;
    dxCommon_ = dxCommon;

    // もしテクスチャが設定されていなかった時用のデフォルトテクスチャを読み込む
    Load("Resources/white1x1.png");

    // 初期化完了のログを出力
    Log("TextureManager Initialized.");
    LogNewLine();
}

TextureManager::~TextureManager() {
    // 終了処理完了のログを出力
    Log("TextureManager Finalized.");
    LogNewLine();
}

uint32_t TextureManager::Load(const std::string &filePath) {
    // 読み込む前に同じ名前のテクスチャがあるか確認
    for (const auto &texture : textures_) {
        if (texture.name == filePath) {
            // すでに読み込まれている場合はそのインデックスを返す
            return static_cast<uint32_t>(&texture - &textures_[0]);
        }
    }

    // テクスチャファイルを読み込んで扱えるようにする
    DirectX::ScratchImage mipImages = LoadTexture(filePath);
    // ミップマップのメタデータを取得
    const DirectX::TexMetadata &metadata = mipImages.GetMetadata();

    // テクスチャデータを作成
    Texture texture = {
        filePath,
        nullptr,
        nullptr,
        // SRVを作成するDescriptorHeapの場所を決める
        SRV::GetCPUDescriptorHandle(),
        SRV::GetGPUDescriptorHandle(),
        // テクスチャのサイズを保存
        static_cast<uint32_t>(metadata.width),
        static_cast<uint32_t>(metadata.height)
    };
    textures_.push_back(texture);

    // テクスチャリソースを作成
    CreateTextureResource(metadata);

    // テクスチャリソースをアップロード
    textures_.back().intermediateResource = UploadTextureData(
        textures_.back().resource.Get(),
        mipImages
    );

    // コマンドを実行
    dxCommon_->CommandExecute(false);

    // metadataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVの生成
    dxCommon_->GetDevice()->CreateShaderResourceView(
        textures_.back().resource.Get(),
        &srvDesc,
        textures_.back().srvHandleCPU
    );

    // Barrierを元に戻す
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = textures_.back().resource.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    dxCommon_->SetBarrier(barrier);

    // 読み込んだテクスチャとそのインデックスをログに出力
    Log(std::format("Load Texture: {} ({}x{}) index: {}",
        filePath,
        textures_.back().width,
        textures_.back().height,
        static_cast<uint32_t>(textures_.size() - 1)
    ), kLogLevelFlagInfo);

    // テクスチャのインデックスを返す
    return static_cast<uint32_t>(textures_.size() - 1);
}

const Texture &TextureManager::GetTexture(uint32_t index) const {
    // インデックスが範囲外の場合は0を返す
    if (index >= textures_.size()) {
        Log("Texture index out of range.", kLogLevelFlagWarning);
        return textures_[0];
    }
    // テクスチャデータを返す
    return textures_[index];
}

void TextureManager::CreateTextureResource(const DirectX::TexMetadata &metadata) {
    //==================================================
    // metadataを基にResourceの設定
    //==================================================

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);                              // Textureの幅
    resourceDesc.Height = UINT(metadata.height);                            // Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels);                    // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);             // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format;                                  // TextureのFormat
    resourceDesc.SampleDesc.Count = 1;                                      // サンプリングカウント
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);  // Textureの次元数

    //==================================================
    // 利用するHeapの設定
    //==================================================

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //==================================================
    // Resourceを生成する
    //==================================================

    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProperties,                            // Heapの設定
        D3D12_HEAP_FLAG_NONE,                       // Heapの特殊な設定
        &resourceDesc,                              // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST,             // データ転送される設定
        nullptr,                                    // Clear最適値。使わないのでnullptr
        IID_PPV_ARGS(&textures_.back().resource)    // 作成するResourceポインタへのポインタ
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages) {
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(
        dxCommon_->GetDevice(),
        mipImages.GetImages(),
        mipImages.GetImageCount(),
        mipImages.GetMetadata(),
        subresources
    );
    uint64_t intermediateSize = GetRequiredIntermediateSize(
        texture,
        0,
        UINT(subresources.size())
    );
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
        PrimitiveDrawer::CreateBufferResources(intermediateSize);
    // データ転送をコマンドに積む
    UpdateSubresources(
        dxCommon_->GetCommandList(),
        texture,
        intermediateResource.Get(),
        0,
        0,
        UINT(subresources.size()),
        subresources.data()
    );

    // Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEに遷移
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    dxCommon_->SetBarrier(barrier);

    return intermediateResource;
}

} // namespace KashipanEngine
