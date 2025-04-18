#include <Windows.h>
#include <cassert>
#include <format>

#include "DirectXCommon.h"
#include "WinApp.h"

namespace MyEngine {

namespace {

// 各エンジン用クラスのグローバル変数
WinApp *sWinApp = nullptr;

/// @brief 深度バッファ用のテクスチャリソースを生成する
/// @param width テクスチャの幅
/// @param height テクスチャの高さ
[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device *device, int32_t width, int32_t height) {
    //==================================================
    // Resourceの設定
    //==================================================

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;                                     // Textureの幅
    resourceDesc.Height = height;                                   // Textureの高さ
    resourceDesc.MipLevels = 1;                                     // mipmapの数
    resourceDesc.DepthOrArraySize = 1;                              // 奥行き or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // DepthStencilとして利用可能なフォーマット
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
    depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f（最大値）でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

    //==================================================
    // Resourceの生成
    //==================================================

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,                    // Heapの設定
        D3D12_HEAP_FLAG_NONE,               // Heapの特殊な設定
        &resourceDesc,                      // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE,   // 深度値を書き込む状態にしておく
        &depthClearValue,                   // Clear最適値
        IID_PPV_ARGS(&resource)             // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));

    return resource;
}

} // namespace

void DirectXCommon::Initialize(bool enableDebugLayer) {
    // 初期化済みかどうかのフラグ
    static bool isInitialized = false;
    // 初期化済みならエラーを出す
    assert(!isInitialized);
    assert(!sWinApp);

    // Windowsアプリクラスのインスタンスを取得
    sWinApp = WinApp::GetInstance();

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
    InitializeRTVDescriptorHeap();  // RTVのディスクリプタヒープ初期化
    InitializeSwapChainResources(); // スワップチェインから取得したリソース初期化
    InitializeRTVHandle();          // RTVのディスクリプタヒープのハンドル初期化
    InitializeFence();              // Fence初期化
    InitializeDepthStencil();       // 深度バッファ初期化

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DirectX.");
}

void DirectXCommon::Finalize() {
    // フェンスのイベントハンドルを閉じる
    CloseHandle(fenceEvent_);

    // 終了処理完了のログを出力
    sWinApp->Log("Complete Finalize DirectX.");
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

    // 描画先のRTVとDSVを設定
    dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    commandList_->OMSetRenderTargets(
        1,
        &rtvHandle_[swapChain_->GetCurrentBackBufferIndex()],
        false,
        &dsvHandle_
    );
    // 深度バッファのクリア
    commandList_->ClearDepthStencilView(
        dsvHandle_,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr
    );
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
    CommandExecute();
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
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandList_->ClearRenderTargetView(rtvHandle_[backBufferIndex], clearColor, 0, nullptr);
}

void DirectXCommon::SetBarrier(D3D12_RESOURCE_BARRIER &barrier) {
    // バリアを張る
    commandList_->ResourceBarrier(1, &barrier);
    
    // Barrierの状態を更新
    currentBarrierState_ = barrier.Transition.StateAfter;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}

void DirectXCommon::InitializeDXGI() {
    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr));

    // アダプタを初期化
    InitializeDXGIAdapter();
    // D3D12デバイスを初期化
    InitializeD3D12Device();

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DXGI.");

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
        assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ使用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 使用するアダプタの情報をログに出力。wstringの方なので変換しておく
            sWinApp->Log(std::format(L"Use Adapter : {}", adapterDesc.Description));
            break;
        }

        // ソフトウェアアダプタの場合は見なかったことにする
        useAdapter_ = nullptr;
    }

    // アダプタが見つからなかった場合はエラー
    assert(useAdapter_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DXGI Adapter.");
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
            sWinApp->Log(std::format("Feature Level : {}", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスが生成できなかった場合はエラー
    assert(device_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize D3D12 Device.");
}

void DirectXCommon::InitializeCommandQueue() {
    // コマンドキューの生成
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Command Queue.");
}

void DirectXCommon::InitializeCommandAllocator() {
    // コマンドアロケータの生成
    commandAllocator_ = nullptr;
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    // コマンドアロケータの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Command Allocator.");
}

void DirectXCommon::InitializeCommandList() {
    // コマンドリストの生成
    commandList_ = nullptr;
    HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    // コマンドリストの生成が成功したかをチェック
    assert(SUCCEEDED(hr));
}

void DirectXCommon::InitializeSwapChain() {
    // スワップチェインの生成
    swapChain_ = nullptr;
    
    swapChainDesc_.Width = sWinApp->GetClientWidth();               // 画面の幅。ウィンドウのクライアント領域と同じにする
    swapChainDesc_.Height = sWinApp->GetClientHeight();             // 画面の高さ。ウィンドウのクライアント領域と同じにする
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;             // 色の形式。8bitのRGBA
    swapChainDesc_.SampleDesc.Count = 1;                            // マルチサンプルしない
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // レンダーターゲットとして使用
    swapChainDesc_.BufferCount = 2;                                 // バッファ数。ダブルバッファ
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // モニタに映したら中身を破棄

    // コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェインを生成
    HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), sWinApp->GetWindowHandle(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1 **>(swapChain_.GetAddressOf()));
    // スワップチェインの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize SwapChain.");
}

void DirectXCommon::InitializeRTVDescriptorHeap() {
    // レンダーターゲットビューのディスクリプタヒープの生成
    rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    
    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize RTV Descriptor Heap.");
}

void DirectXCommon::InitializeSwapChainResources() {
    // スワップチェインからのリソースの生成
    for (UINT i = 0; i < 2; ++i) {
        swapChainResources_[i] = nullptr;
        HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
        // リソースの生成が成功したかをチェック
        assert(SUCCEEDED(hr));
    }

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize SwapChain Resources.");
}

void DirectXCommon::InitializeRTVHandle() {
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;       // 出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  // 2Dテクスチャとして書き込む

    // ディスクリプタの先頭を取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    // RTVの1つ目を作成。1つ目は最初のところに作る。作る場所をこちらで設定してあげる必要がある
    rtvHandle_[0] = rtvStartHandle;
    device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandle_[0]);

    // 2つ目のディスクリプタハンドルを得る(自力で)
    rtvHandle_[1].ptr = rtvHandle_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    // RTVの2つ目を作成。
    device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandle_[1]);
}

void DirectXCommon::InitializeFence() {
    // フェンスの生成。初期値0で作る
    fence_ = nullptr;
    fenceValue_ = 0;
    HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    // フェンスの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // フェンスのSignalを持つためのイベントを作成する
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    // フェンスのイベントハンドルの生成が成功したかをチェック
    assert(fenceEvent_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Fence.");
}

void DirectXCommon::InitializeDepthStencil() {
    // 深度バッファ用のテクスチャリソースを作成
    depthStencilResource_ = CreateDepthStencilTextureResource(
        device_.Get(),
        sWinApp->GetClientWidth(),
        sWinApp->GetClientHeight()
    );

    // DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dsvDescriptorHeap_ = CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        1,
        false
    );

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;         // Format。基本的にはResourceと合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
    // DSVHeapの先頭にDSVを作成
    device_->CreateDepthStencilView(
        depthStencilResource_.Get(),
        &dsvDesc,
        dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart()
    );

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Depth Stencil.");
}

void DirectXCommon::CommandExecute() {
    // コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
    HRESULT hr = commandList_->Close();
    // コマンドリストの内容を確定できたかをチェック
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList *commandLists[] = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists);
    // GPUとOSに画面の交換を行うよう通知
    swapChain_->Present(1, 0);

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
    assert(SUCCEEDED(hr));
    hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

DirectXCommon::D3DResourceLeakChecker::~D3DResourceLeakChecker() {
    // リソースリークチェック
    Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
    }
}

} // namespace MyEngine