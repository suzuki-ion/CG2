#pragma once

// 基底クラス
#include "Graphics/IGPUResource.h"

// 各リソースクラス
#include "Graphics/GPUResources/RenderTargetResource.h"
#include "Graphics/GPUResources/DepthStencilResource.h"
#include "Graphics/GPUResources/ShaderResourceResource.h"
#include "Graphics/GPUResources/ConstantBufferResource.h"
#include "Graphics/GPUResources/VertexBufferResource.h"
#include "Graphics/GPUResources/IndexBufferResource.h"
#include "Graphics/GPUResources/UnorderedAccessResource.h"

/// @brief KashipanEngine GPU Resources
/// 
/// 使用例:
/// 
/// // 初期化（アプリケーション開始時に一度だけ）
/// IGPUResource::InitializeCommon(dxCommon);
/// 
/// // レンダーターゲット作成
/// auto renderTarget = std::make_unique<RenderTargetResource>("MainRT", 1280, 720);
/// renderTarget->Create();
/// 
/// // 定数バッファ作成
/// auto constantBuffer = std::make_unique<ConstantBufferResource<TransformMatrix>>("Transform");
/// constantBuffer->Create();
/// 
/// // 頂点バッファ作成
/// auto vertexBuffer = std::make_unique<VertexBufferResource<Vertex>>("Vertices", 100);
/// vertexBuffer->Create();