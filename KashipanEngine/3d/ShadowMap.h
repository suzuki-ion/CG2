#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#include "Common/PipeLineSet.h"

namespace KashipanEngine {

// 前方宣言
class DirectXCommon;
struct Object;
struct DirectionalLight;

/// @brief シャドウマップ用クラス
class ShadowMap {
public:
    ShadowMap(DirectXCommon *dxCommon);

    void PreDraw();
    void PostDraw();

    void Draw(std::vector<Object *> &objects);
    void SetLight(DirectionalLight *light);
    
    /// @brief シャドウマップのリソースを取得
    ID3D12Resource *GetResource() const {
        return resource_.Get();
    }

private:
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;

    /// @brief 平行光源へのポインタ
    DirectionalLight *directionalLight_ = nullptr;

    /// @brief シャドウマップのリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    /// @brief シャドウマップのリソースのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE handle_;
    /// @brief シャドウマップ用のパイプラインセット
    PipeLineSet pipelineSet_;

    /// @brief シャドウマップのビュー
    D3D12_VIEWPORT viewport_{};
    /// @brief シャドウマップのシザー矩形
    D3D12_RECT scissorRect_{};
};

} // namespace KashipanEngine