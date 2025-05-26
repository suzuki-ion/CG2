#pragma once
#include <wrl.h>
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace KashipanEngine {

// 前方宣言
class WinApp;
struct Vector4;

/// @brief DirectX共通クラス
class DirectXCommon final {
public:
    DirectXCommon(bool enableDebugLayer, WinApp *winApp);
    ~DirectXCommon();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();

    /// @brief レンダーターゲットのクリア
    void ClearRenderTarget();
    /// @brief 深度バッファのクリア
    void ClearDepthStencil();

    /// @brief バリアを張る
    /// @param barrier バリアの設定
    void SetBarrier(D3D12_RESOURCE_BARRIER &barrier);

    /// @brief レンダーターゲットのクリア色を設定
    /// @param r 赤
    /// @param g 緑
    /// @param b 青
    /// @param a 透明度
    void SetClearColor(float r, float g, float b, float a);

    /// @brief レンダーターゲットのクリア色を設定
    /// @param color RGBA
    void SetClearColor(const Vector4 &color);

    /// @brief ウィンドウサイズ変更適応
    void Resize();

    /// @brief コマンドの実行
    /// @param isSwapChain スワップチェインの実行かどうか
    void CommandExecute(bool isSwapChain);

    /// @brief ディスクリプタヒープの作成
    /// @param descriptorHeap ディスクリプタヒープへのポインタ
    /// @param heapType ヒープの種類
    /// @param numDescriptors ディスクリプタの数
    /// @param shaderVisible シェーダーから見えるかどうか
    void CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible = false);

    /// @brief 深度バッファのリソースを作成
    /// @param width 深度バッファの幅
    /// @param height 深度バッファの高さ
    /// @param format フォーマット
    /// @param initialState 初期状態
    /// @return 深度バッファのリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height, DXGI_FORMAT format, D3D12_RESOURCE_STATES initialState);

    /// @brief D3D12デバイス取得
    /// @return D3D12デバイス
    ID3D12Device *GetDevice() const { return device_.Get(); }

    /// @brief コマンドキュー取得
    /// @return コマンドキュー
    ID3D12CommandQueue *GetCommandQueue() const { return commandQueue_.Get(); }

    /// @brief 描画コマンドリスト取得
    /// @return 描画コマンドリスト
    ID3D12GraphicsCommandList *GetCommandList() const { return commandList_.Get(); }

    /// @brief スワップチェインの設定取得
    /// @return スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }

    /// @brief RTVの設定取得
    /// @return RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }

    /// @brief 現在のBarrierState取得
    /// @return 現在のBarrierState
    D3D12_RESOURCE_STATES GetCurrentBarrierState() const { return currentBarrierState_; }

private:
    //--------- WinApp ---------//

    /// @brief WinAppクラス(外部から持ってくる)
    WinApp *winApp_ = nullptr;

    //--------- DXGI ---------//
    
    /// @brief DXGIファクトリー
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    /// @brief 使用するアダプタ
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
    /// @brief D3D12デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    //--------- コマンド ---------//

    /// @brief コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    /// @brief コマンドアロケータ
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    /// @brief コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    //--------- スワップチェイン ---------//

    /// @brief スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
    /// @brief スワップチェイン
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    /// @brief スワップチェインから取得したリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

    //--------- レンダーターゲット ---------//

    /// @brief レンダーターゲットビューの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
    /// @brief RTVのディスクリプタヒープのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_[2];

    //--------- フェンス ---------//

    /// @brief フェンス
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    /// @brief フェンスの値
    UINT64 fenceValue_;
    /// @brief フェンスのイベントハンドル
    HANDLE fenceEvent_;

    //--------- 深度バッファ ---------//

    /// @brief 深度バッファのリソースのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

    //--------- バリア ---------//

    /// @brief 現在のBarrierState
    D3D12_RESOURCE_STATES currentBarrierState_;

    //--------- 色 ---------//

    /// @brief レンダーターゲットのクリアカラー
    float clearColor_[4] = { 0.1f, 0.25f, 0.5f, 1.0f };

    //--------- 各種初期化用関数 ---------//

    /// @brief DXGI初期化
    void InitializeDXGI();
    /// @brief DXGIアダプター初期化
    void InitializeDXGIAdapter();
    /// @brief D3D12デバイス初期化
    void InitializeD3D12Device();

    /// @brief コマンドキュー初期化
    void InitializeCommandQueue();
    /// @brief コマンドアロケータ初期化
    void InitializeCommandAllocator();
    /// @brief コマンドリスト初期化
    void InitializeCommandList();

    /// @brief スワップチェイン初期化
    void InitializeSwapChain();
    /// @brief RTVの初期化
    void InitializeRTV();
    /// @brief DSVの初期化
    void InitializeDSV();
    /// @brief スワップチェインから取得したリソース初期化
    void InitializeSwapChainResources();
    /// @brief RTVのディスクリプタヒープのハンドル初期化
    void InitializeRTVHandle();
    /// @brief DSVのディスクリプタヒープのハンドル初期化
    void InitializeDSVHandle();

    /// @brief フェンス初期化
    void InitializeFence();
};

} // namespace KashipanEngine