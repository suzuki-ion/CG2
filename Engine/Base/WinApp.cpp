#include <cassert>
#include <format>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <2d/ImGuiManager.h>

#include "WinApp.h"
#include "ConvertString.h"
#include "TimeGet.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace MyEngine {

namespace {
// ログ出力用のストリーム
std::ofstream logStream;
} // namespace

void WinApp::Initialize(const std::wstring &title, UINT windowStyle, int32_t width, int32_t height) {
    // 初期化済みかどうかのフラグ
    static bool isInitialized = false;
    // 初期化済みで再度実行されたらエラーを出す
    assert(!isInitialized);

    //==================================================
    // ログファイルの準備
    //==================================================

    // ログファイルのパスを作成
    std::filesystem::create_directory("Logs");
    // 時刻を使ってファイル名を決定
    std::string logFilePath = "Logs/" + TimeGetString("{:%Y-%m-%d_%H-%M-%S}") + ".log";
    // ファイルを使って書き込み準備
    logStream.open(logFilePath);

    //==================================================
    // ウィンドウの初期化
    //==================================================
    
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = L"Window";
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // ウィンドウクラスを登録
    RegisterClass(&wc_);

    // クライアントサイズの横幅
    kClientWidth_ = width;
    // クライアントサイズの縦幅
    kClientHeight_ = height;

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    wrc_ = { 0, 0, kClientWidth_, kClientHeight_ };

    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウハンドルを作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,      // 利用するクラス名
        title.c_str(),          // タイトルバーの文字
        windowStyle,            // ウィンドウスタイル
        CW_USEDEFAULT,          // 表示X座標
        CW_USEDEFAULT,          // 表示Y座標
        wrc_.right - wrc_.left, // ウィンドウ横幅
        wrc_.bottom - wrc_.top, // ウィンドウ縦幅
        nullptr,                // 親ウィンドウハンドル
        nullptr,                // メニューハンドル
        wc_.hInstance,          // インスタンスハンドル
        nullptr                 // オプション
    );

    // ウィンドウを表示
    ShowWindow(hwnd_, SW_SHOW);

    // 初期化完了のログを出力
    Log("Complete Initialize WinApp.");
}

void WinApp::Finalize() {
    // ウィンドウを破棄
    DestroyWindow(hwnd_);
    // ウィンドウクラスを登録解除
    UnregisterClass(wc_.lpszClassName, wc_.hInstance);

    // 終了処理完了のログを出力
    Log("Complete Finalize WinApp.");
}

int WinApp::ProccessMessage() {
    if (msg_.message == WM_QUIT) {
        Log("Window Quit.");
        return -1;
    }

    if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);

    } else {
        return 0;
    }
}

void WinApp::Log(const std::string &message) {
    // ログファイルに書き込み
    logStream << TimeGetString("[ {:%Y/%m/%d %H:%M:%S} ] : ") << message << std::endl;
    OutputDebugStringA((message + '\n').c_str());
}

void WinApp::Log(const std::wstring &message) {
    // ログファイルに書き込み
    logStream << TimeGetString("[ {:%Y/%m/%d %H:%M:%S} ] : ") << ConvertString(message) << std::endl;
    // OutputDebugStringWでもいいらしいけど、よくわからんので変換で対応
    OutputDebugStringA(ConvertString(message + L'\n').c_str());
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
    
    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された時
        case WM_DESTROY:
            // OSに対して。アプリの終了を伝える
            PostQuitMessage(0);
            return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

} // namespace MyEngine