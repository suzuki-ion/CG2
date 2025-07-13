#pragma once
#include <Windows.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <source_location>
#include <format>

#include "Common/Mesh.h"
#include "Common/PipeLineSet.h"
#include "Common/Logs.h"
#include "Common/VertexData.h"

namespace KashipanEngine {

enum BlendMode {
    kBlendModeNone,
    kBlendModeNormal,
    kBlendModeAdd,
    kBlendModeSubtract,
    kBlendModeMultiply,
    kBlendModeScreen,
    kBlendModeExclusion,

    kBlendModeMax,
};

enum FillMode {
    kFillModeSolid,
    kFillModeWireframe,
};

// 前方宣言
class DirectXCommon;

/*
シングルトンでどこからでもアクセスできてしまうため、
必ずどこから呼び出されたかを特定するために引数にsource_locationを設定
*/

/// @brief プリミティブ描画クラス
class PrimitiveDrawer final {
public:
    PrimitiveDrawer(const PrimitiveDrawer &) = delete;
    PrimitiveDrawer(const PrimitiveDrawer &&) = delete;
    PrimitiveDrawer &operator=(const PrimitiveDrawer &) = delete;
    PrimitiveDrawer &operator=(const PrimitiveDrawer &&) = delete;

    /// @brief 初期化処理
    /// @param dxCommon DirectXCommonインスタンスへのポインタ
    static void Initialize(DirectXCommon *dxCommon, const std::source_location &location = std::source_location::current());

    /// @brief リソース生成
    /// @param size サイズ
    /// @return 生成したリソース
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResources(UINT64 size, const std::source_location &location = std::source_location::current());

    /// @brief メッシュ生成
    /// @param vertexCount 頂点数
    /// @param indexCount インデックス数
    /// @param vertexStride 1頂点あたりのバイト数
    /// @return 生成したメッシュ
    template<typename T>
    static std::unique_ptr<Mesh<T>> CreateMesh(
        UINT vertexCount,
        UINT indexCount,
        unsigned long long vertexStride = sizeof(VertexData),
        const std::source_location &location = std::source_location::current()) {
        // テンプレート型を使っていてcppに記述できない部分はここで定義

        // 中間メッシュを生成
        IntermediateMesh intermediateMesh = CreateIntermediateMesh(
            vertexCount, indexCount, vertexStride, location);
        // メッシュを返す
        auto mesh = std::make_unique<Mesh<T>>();
        mesh->vertexBuffer      = intermediateMesh.vertexBuffer;
        mesh->indexBuffer       = intermediateMesh.indexBuffer;
        mesh->vertexBufferView  = intermediateMesh.vertexBufferView;
        mesh->indexBufferView   = intermediateMesh.indexBufferView;
        // マップを取得
        mesh->vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&mesh->vertexBufferMap));
        mesh->indexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&mesh->indexBufferMap));
        return mesh;
    }
    
    /// @brief グラフィックパイプライン生成
    /// @param topologyType トポロジータイプ
    /// @param blendMode ブレンドモード
    /// @return パイプラインセット
    static PipeLineSet CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode, const bool isDepthEnable = true, const std::source_location &location = std::source_location::current());

    /// @brief 線用のパイプライン生成
    /// @return 線用のパイプラインセット
    static PipeLineSet CreateLinePipeline(const std::source_location &location = std::source_location::current());

private:
    PrimitiveDrawer() = default;
    ~PrimitiveDrawer() = default;

    /// @brief 中間メッシュ構造体
    struct IntermediateMesh {
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;    // 頂点バッファ
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;     // インデックスバッファ
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;              // 頂点バッファビュー
        D3D12_INDEX_BUFFER_VIEW indexBufferView;                // インデックスバッファビュー
    };

    static IntermediateMesh CreateIntermediateMesh(
        UINT vertexCount,
        UINT indexCount,
        unsigned long long vertexStride,
        const std::source_location &location);

    /// @brief 初期化フラグ
    static bool isInitialized_;
    /// @brief DirectXCommonインスタンス
    static DirectXCommon *dxCommon_;
};

} // namespace KashipanEngine