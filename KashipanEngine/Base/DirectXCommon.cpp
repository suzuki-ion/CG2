#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Windows.h>
#include <cassert>
#include <format>

#include "DirectXCommon.h"
#include "WinApp.h"
#include "Common/Logs.h"

#include "Common/Descriptors/RTV.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"
#include "Math/Vector4.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace KashipanEngine {

DirectXCommon::DirectXCommon(bool enableDebugLayer, WinApp *winApp) {
    // nullチェック
    if (!winApp) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    // WinAppクラスへのポインタを設定
    winApp_ = winApp;

#ifdef _DEBUG
    ID3D12Debug1 *debugController = nullptr;
    if (enableDebugLayer) {
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            // デバッグレイヤーを有効にする
            debugController->EnableDebugLayer();
            // さらにGPU側でもチェックを行うようにする
            debugController->SetEnableGPUBasedValidation(true);
        }
    }
#else
    // リリースビルドの場合は何もしない
    static_cast<void>(enableDebugLayer);
#endif

    //==================================================
    // DirectXの初期化
    //==================================================

    InitializeDXGI();               // DXGI初期化
    InitializeCommandQueue();       // コマンドキュー初期化
    InitializeCommandAllocator();   // コマンドアロケータ初期化
    InitializeCommandList();        // コマンドリスト初期化
    InitializeSwapChain();          // スワップチェイン初期化
    InitializeRTV();                // RTVの初期化
    InitializeDSV();                // DSVの初期化
    InitializeSwapChainResources(); // スワップチェインから取得したリソース初期化
    InitializeRTVHandle();          // RTVのディスクリプタヒープのハンドル初期化
    InitializeDSVHandle();          // DSVのディスクリプタヒープのハンドル初期化
    InitializeFence();              // Fence初期化

    // 初期化完了のログを出力
    Log("DirectXCommon Initialized.");
    LogNewLine();
}

DirectXCommon::~DirectXCommon() {
    // フェンスのイベントハンドルを閉じる
    CloseHandle(fenceEvent_);
    winApp_ = nullptr;

    // 終了処理完了のログを出力
    Log("DirectXCommon Finalized.");
    LogNewLine();
}

void DirectXCommon::PreDraw() {
    // これから書き込むバックバッファのindexを取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // TransitionBarrierを張る
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
    barrier.Transition.StateBefore = currentBarrierState_;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    SetBarrier(barrier);

    // レンダーターゲットのクリア
    ClearRenderTarget();
}

void DirectXCommon::PostDraw() {
    // これから書き込むバックバッファのindexを取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    // 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
    // 今回はRenderTargetからPresentにする
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
    barrier.Transition.StateBefore = currentBarrierState_;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    SetBarrier(barrier);

    // コマンドの実行
    CommandExecute(true);
}

void DirectXCommon::ClearRenderTarget() {
    // これから書き込むバックバッファのindexを取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    //==================================================
    // 画面クリア処理
    //==================================================

    // 描画先のRTVを設定
    commandList_->OMSetRenderTargets(1, &rtvHandle_[backBufferIndex], false, nullptr);
    // 指定した色で画面全体をクリア
    commandList_->ClearRenderTargetView(rtvHandle_[backBufferIndex], clearColor_, 0, nullptr);
}

void DirectXCommon::ClearDepthStencil() {
    commandList_->ClearDepthStencilView(
        dsvHandle_,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr
    );
    // 描画先のRTVとDSVを設定
    commandList_->OMSetRenderTargets(
        1,
        &rtvHandle_[swapChain_->GetCurrentBackBufferIndex()],
        false,
        &dsvHandle_
    );
}

void DirectXCommon::SetBarrier(D3D12_RESOURCE_BARRIER &barrier) {
    // バリアを張る
    commandList_->ResourceBarrier(1, &barrier);
    
    // Barrierの状態を更新
    currentBarrierState_ = barrier.Transition.StateAfter;
}

void DirectXCommon::SetClearColor(float r, float g, float b, float a) {
    clearColor_[0] = r;
    clearColor_[1] = g;
    clearColor_[2] = b;
    clearColor_[3] = a;
}

void DirectXCommon::SetClearColor(const Vector4 &color) {
    clearColor_[0] = color.x;
    clearColor_[1] = color.y;
    clearColor_[2] = color.z;
    clearColor_[3] = color.w;
}

void DirectXCommon::Resize() {
    for (UINT i = 0; i < 2; ++i) {
        swapChainResources_[i] = nullptr;
    }
    // スワップチェインのサイズを変更
    HRESULT hr = swapChain_->ResizeBuffers(
        2,
        static_cast<UINT>(winApp_->GetClientWidth()),
        static_cast<UINT>(winApp_->GetClientHeight()),
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    );
    // スワップチェインのサイズ変更が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    // スワップチェインからのリソースを再生成
    InitializeSwapChainResources();
    // RTVの初期化
    RTV::Initialize(this);
    // DSVの初期化
    DSV::Initialize(winApp_, this);
    // RTVハンドルの初期化
    InitializeRTVHandle();
    // DSVハンドルの初期化
    InitializeDSVHandle();
}

void DirectXCommon::CommandExecute(bool isSwapChain) {
    // コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
    HRESULT hr = commandList_->Close();
    // コマンドリストの内容を確定できたかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList *commandLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists);

    // スワップチェインの実行を行うかどうか
    if (isSwapChain) {
        // スワップチェインの実行を行う
        swapChain_->Present(1, 0);
    }

    //==================================================
    // Fenceの処理
    //==================================================

    // Fenceの値を更新
    fenceValue_++;
    // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < fenceValue_) {
        // 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }

    // 次のフレーム用のコマンドリストを準備
    hr = commandAllocator_->Reset();
    if (FAILED(hr)) assert(SUCCEEDED(hr));
    hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
    descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(int32_t width, int32_t height, DXGI_FORMAT format, D3D12_RESOURCE_STATES initialState) {
    //==================================================
    // Resourceの設定
    //==================================================

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;                                     // Textureの幅
    resourceDesc.Height = height;                                   // Textureの高さ
    resourceDesc.MipLevels = 1;                                     // mipmapの数
    resourceDesc.DepthOrArraySize = 1;                              // 奥行き or 配列Textureの配列数
    resourceDesc.Format = format;                                   // フォーマット
    resourceDesc.SampleDesc.Count = 1;                              // サンプリングカウント数。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;    // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;   // DepthStencilとして使う通知

    //==================================================
    // 利用するHeapの設定
    //==================================================

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //==================================================
    // 深度値のクリア最適化設定
    //==================================================

    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Stencil = 0;   // ステンシル値
    depthClearValue.DepthStencil.Depth = 1.0f;  // 1.0f（最大値）でクリア
    depthClearValue.Format = format;            // フォーマット。Resourceと合わせる

    //==================================================
    // Resourceの生成
    //==================================================

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device_->CreateCommittedResource(
        &heapProperties,        // Heapの設定
        D3D12_HEAP_FLAG_NONE,   // Heapの特殊な設定
        &resourceDesc,          // Resourceの設定
        initialState,           // 深度値を書き込む状態にしておく
        &depthClearValue,       // Clear最適値
        IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
    );
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    return resource;
}

void DirectXCommon::InitializeDXGI() {
    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // アダプタを初期化
    InitializeDXGIAdapter();
    // D3D12デバイスを初期化
    InitializeD3D12Device();

    // 初期化完了のログを出力
    Log("Complete Initialize DXGI.");

    // DirectX12のエラーが出た際に止まるようにする
#ifdef _DEBUG
    ID3D12InfoQueue *infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告の時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
#endif
}

void DirectXCommon::InitializeDXGIAdapter() {
    // 使用するアダプタ用の変数をnullptrで初期化しておく
    useAdapter_ = nullptr;

    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);

        // アダプタの情報を取得できたかどうかをチェック
        if (FAILED(hr)) assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ使用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 使用するアダプタの情報をログに出力。wstringの方なので変換しておく
            Log(std::format(L"Use Adapter : {}", adapterDesc.Description));
            break;
        }

        // ソフトウェアアダプタの場合は見なかったことにする
        useAdapter_ = nullptr;
    }

    // アダプタが見つからなかった場合はエラー
    assert(useAdapter_ != nullptr);

    // 初期化完了のログを出力
    Log("Complete Initialize DXGI Adapter.");
}

void DirectXCommon::InitializeD3D12Device() {
    // D3D12デバイスの生成
    device_ = nullptr;
    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    const char *featureLevelStrings[] = {
        "12.2",
        "12.1",
        "12.0"
    };

    // 高い順に生成できるか試す
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 使用してるアダプタでデバイスを生成
        HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスを生成できたかをチェック
        if (SUCCEEDED(hr)) {
            // 生成できたらログに出力して終了
            Log(std::format("Feature Level : {}", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスが生成できなかった場合はエラー
    assert(device_ != nullptr);

    // 初期化完了のログを出力
    Log("Complete Initialize D3D12 Device.");
}

void DirectXCommon::InitializeCommandQueue() {
    // コマンドキューの生成
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    Log("Complete Initialize Command Queue.");
}

void DirectXCommon::InitializeCommandAllocator() {
    // コマンドアロケータの生成
    commandAllocator_ = nullptr;
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    // コマンドアロケータの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    Log("Complete Initialize Command Allocator.");
}

void DirectXCommon::InitializeCommandList() {
    // コマンドリストの生成
    commandList_ = nullptr;
    HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    // コマンドリストの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));
}

void DirectXCommon::InitializeSwapChain() {
    // スワップチェインの生成
    swapChain_ = nullptr;
    
    swapChainDesc_.Width = winApp_->GetClientWidth();               // 画面の幅。ウィンドウのクライアント領域と同じにする
    swapChainDesc_.Height = winApp_->GetClientHeight();             // 画面の高さ。ウィンドウのクライアント領域と同じにする
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;             // 色の形式。8bitのRGBA
    swapChainDesc_.SampleDesc.Count = 1;                            // マルチサンプルしない
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // レンダーターゲットとして使用
    swapChainDesc_.BufferCount = 2;                                 // バッファ数。ダブルバッファ
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // モニタに映したら中身を破棄
    swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // モード変更を許可

    // コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェインを生成
    HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetWindowHandle(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1 **>(swapChain_.GetAddressOf()));
    // スワップチェインの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    Log("Complete Initialize SwapChain.");
}

void DirectXCommon::InitializeRTV() {
    // レンダーターゲットビューの初期化
    RTV::Initialize(this);

    // 初期化完了のログを出力
    Log("Complete Initialize RTV.");
}

void DirectXCommon::InitializeDSV() {
    // 深度ステンシルビューの初期化
    DSV::Initialize(winApp_, this);

    // 初期化完了のログを出力
    Log("Complete Initialize DSV.");
}

void DirectXCommon::InitializeSwapChainResources() {
    static bool isInitialized = false;

    // スワップチェインからのリソースの生成
    for (UINT i = 0; i < 2; ++i) {
        swapChainResources_[i] = nullptr;
        HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
        // リソースの生成が成功したかをチェック
        if (FAILED(hr)) assert(SUCCEEDED(hr));
    }
    
    if (!isInitialized) {
        // 初期化完了のログを出力
        Log("Complete Initialize SwapChain Resources.");
    } else {
        // 2回目以降は再生成のログを出力
        Log("Reinitialize SwapChain Resources.");
    }
}

void DirectXCommon::InitializeRTVHandle() {
    static bool isInitialized = false;

    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;       // 出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  // 2Dテクスチャとして書き込む

    // RTVの1つ目を作成。1つ目は最初のところに作る。作る場所をこちらで設定してあげる必要がある
    rtvHandle_[0] = RTV::GetCPUDescriptorHandle(0);
    device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandle_[0]);

    // 2つ目のディスクリプタハンドルを得る
    rtvHandle_[1] = RTV::GetCPUDescriptorHandle(1);
    // RTVの2つ目を作成。
    device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandle_[1]);

    // 初期化完了のログを出力
    if (!isInitialized) {
        Log("Complete Initialize RTV Handle.");
        isInitialized = true;
    } else {
        Log("Reinitialize RTV Handle.");
    }
}

void DirectXCommon::InitializeDSVHandle() {
    static bool isInitialized = false;

    dsvHandle_ = DSV::GetCPUDescriptorHandle(0);
    
    // 初期化完了のログを出力
    if (!isInitialized) {
        Log("Complete Initialize DSV Handle.");
        isInitialized = true;
    } else {
        Log("Reinitialize DSV Handle.");
    }
}

void DirectXCommon::InitializeFence() {
    // フェンスの生成。初期値0で作る
    fence_ = nullptr;
    fenceValue_ = 0;
    HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    // フェンスの生成が成功したかをチェック
    if (FAILED(hr)) assert(SUCCEEDED(hr));

    // フェンスのSignalを持つためのイベントを作成する
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    // フェンスのイベントハンドルの生成が成功したかをチェック
    assert(fenceEvent_ != nullptr);

    // 初期化完了のログを出力
    Log("Complete Initialize Fence.");
}

} // namespace KashipanEngine