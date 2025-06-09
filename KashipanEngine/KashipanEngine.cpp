#define NOMINMAX

#include <cassert>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>

#include "Common/ConvertString.h"
#include "Common/VertexData.h"
#include "Common/ConvertColor.h"
#include "Common/Logs.h"
#include "Common/Descriptors/RTV.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Texture.h"
#include "Base/CrashHandler.h"
#include "Base/ResourceLeakChecker.h"
#include "Base/Renderer.h"
#include "Base/Input.h"
#include "Base/Sound.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Camera.h"
#include "Math/RenderingPipeline.h"
#include "Objects/Object.h"
#include "Objects/Model.h"
#include "KashipanEngine.h"

using namespace KashipanEngine;

namespace {

/// @brief 終了処理チェック用構造体
struct FinalizeChecker {
    ~FinalizeChecker();
};
/// @brief 終了処理チェック用変数
FinalizeChecker finalizeCheck_;
/// @brief リソースリークチェック用変数
D3DResourceLeakChecker leakCheck_;

// 各エンジン用クラスのグローバル変数
std::unique_ptr<WinApp> sWinApp;
std::unique_ptr<DirectXCommon> sDxCommon;
std::unique_ptr<ImGuiManager> sImGuiManager;
std::unique_ptr<Renderer> sRenderer;

// フレーム時間計算用変数
LONGLONG sFrequency = 0;
LONGLONG sElapsedTime = 0;
LONGLONG sLastTime = 0;
LARGE_INTEGER sNowTime;
unsigned int sCountFps = 0;
float sElapsedSeconds = 0.0f;
float sDeltaTime = 0.0f;
const float kMaxDeltaTime = 0.1f;

} // namespace

Engine::Engine(const char *title, int width, int height, bool enableDebugLayer,
    const std::filesystem::path &projectDir) {
    // ログの初期化
    InitializeLog("Logs", projectDir.string());
    LogInsertPartition("\n================ Engine Initialize ===============\n");

    // フレーム時間の初期化
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    sFrequency = freq.QuadPart;

    // COMの初期化
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        Log("Failed to initialize COM library.");
        assert(SUCCEEDED(hr));
    }

    // 誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // タイトル名がそのままだと使えないので変換
    std::wstring wTitle = ConvertString(title);
    // Windowsアプリ初期化
    sWinApp = std::make_unique<WinApp>(
        wTitle,
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        width,
        height
    );
    sWinApp->SetSizeChangeMode(KashipanEngine::SizeChangeMode::kNone);

    // DirectX初期化
    sDxCommon = std::make_unique<DirectXCommon>(enableDebugLayer, sWinApp.get());

    // InputManager初期化
    Input::Initialize(sWinApp.get());

    // プリミティブ描画クラス初期化
    PrimitiveDrawer::Initialize(sDxCommon.get());

    // 音声初期化
    Sound::Initialize();

    // カメラの初期化
    Camera::Initialize(sWinApp.get());

    // ImGui初期化
    sImGuiManager = std::make_unique<ImGuiManager>(sWinApp.get(), sDxCommon.get());

    // テクスチャ管理クラス初期化
    Texture::Initialize(sDxCommon.get());

    // 描画用クラス初期化
    sRenderer = std::make_unique<Renderer>(sWinApp.get(), sDxCommon.get(), sImGuiManager.get());

    // 初期化完了のログを出力
    Log("Engine Initialized.");
    LogInsertPartition("\n============ Engine Initialize Finish ============\n");
}

Engine::~Engine() {
    LogInsertPartition("\n================= Engine Finalize ================\n");
    sRenderer.reset();
    Sound::Finalize();
    Texture::Finalize();
    sImGuiManager.reset();
    sDxCommon.reset();
    sWinApp.reset();
    // DescriptorHeapの解放
    RTV::Finalize();
    DSV::Finalize();
    SRV::Finalize();
    CoUninitialize();
    // 終了処理完了のログを出力
    Log("Engine Finalized.");
    LogInsertPartition("\n============= Engine Finalize Finish =============\n");
}

void Engine::BeginFrame() {
    // ウィンドウのサイズ変更がされてたら他の場所でも適応させる
    if (sWinApp->IsSizing()) {
        sDxCommon->Resize();
    }
    // 時間取得
    QueryPerformanceCounter(&sNowTime);
    // 時間計算
    sElapsedTime = sNowTime.QuadPart - sLastTime;
    sElapsedSeconds = static_cast<float>(sElapsedTime) / sFrequency;
    sDeltaTime = std::min(sElapsedSeconds, kMaxDeltaTime);
}

bool Engine::BeginGameLoop(int frameRate) {
    // モニターのフレームレートを取得
    HDC hdc = GetDC(sWinApp->GetWindowHandle());
    int monitorFrameRate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(sWinApp->GetWindowHandle(), hdc);

    // フレームレートが24以下やモニターのFPS以上なら垂直同期
    if (frameRate < 24 || frameRate > monitorFrameRate) {
        frameRate = monitorFrameRate;
    }

    if (sDeltaTime > 1.0f / static_cast<float>(frameRate)) {
        sLastTime = sNowTime.QuadPart;
        sCountFps = static_cast<unsigned int>(1.0f / sDeltaTime);
        return true;
    }
    return false;
}

void Engine::EndFrame() {
}

float Engine::GetDeltaTime() const {
    return sDeltaTime;
}

unsigned int Engine::GetFPS() const {
    // 補正値として1を入れておく
    return sCountFps + 1;
}

KashipanEngine::WinApp *Engine::GetWinApp() const {
    return sWinApp.get();
}

KashipanEngine::DirectXCommon *Engine::GetDxCommon() const {
    return sDxCommon.get();
}

KashipanEngine::Renderer *Engine::GetRenderer() const {
    return sRenderer.get();
}

int Engine::ProccessMessage() {
    return sWinApp->ProccessMessage();
}

FinalizeChecker::~FinalizeChecker() {
    // エンジンが完全に終了しているかチェック
    assert(!sWinApp);
    assert(!sDxCommon);
    assert(!sImGuiManager);
    // 初期化完了のログを出力
    Log("Complete Finalize Engine.");
}