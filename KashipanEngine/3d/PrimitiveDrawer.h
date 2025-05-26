#pragma once
#include <Windows.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <source_location>

#include "Common/Mesh.h"
#include "Common/PipeLineSet.h"

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
    /// @return 生成したメッシュ
    static std::unique_ptr<Mesh> CreateMesh(UINT vertexCount, UINT indexCount, const std::source_location &location = std::source_location::current());
    
    /// @brief グラフィックパイプライン生成
    /// @param topologyType トポロジータイプ
    /// @param blendMode ブレンドモード
    /// @return パイプラインセット
    static PipeLineSet CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode, const bool isDepthEnable = true, const std::source_location &location = std::source_location::current());

    /// @brief シャドウマップ用のパイプライン生成
    /// @return シャドウマップ用のパイプラインセット
    static PipeLineSet CreateShadowMapPipeline(const std::source_location &location = std::source_location::current());

private:
    PrimitiveDrawer() = default;
    ~PrimitiveDrawer() = default;

    /// @brief 初期化フラグ
    static bool isInitialized_;
    /// @brief DirectXCommonインスタンス
    static DirectXCommon *dxCommon_;
};

} // namespace KashipanEngine