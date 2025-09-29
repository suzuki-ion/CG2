#include "Window.h"
#include <cassert>

namespace KashipanEngine {

Window::Window(const std::wstring& title, int32_t width, int32_t height, DWORD windowStyle)
    : title_(title)
    , clientWidth_(width)
    , clientHeight_(height)
    , windowStyle_(windowStyle)
    , hInstance_(GetModuleHandle(nullptr))
    , isVisible_(false)
    , isActive_(false)
    , isResizing_(false)
    , isMinimized_(false)
    , isMaximized_(false)
    , isSizing_(false) {
    
    // ウィンドウの初期化を実行
    bool result = InitializeWindow(title, width, height, windowStyle);
    assert(result && "Window initialization failed");
}

Window::~Window() {
    Cleanup();
}

bool Window::InitializeWindow(const std::wstring& title, int32_t width, int32_t height, DWORD windowStyle) {
    // パラメータの保存
    title_ = title;
    clientWidth_ = width;
    clientHeight_ = height;
    windowStyle_ = windowStyle;
    hInstance_ = GetModuleHandle(nullptr);

    // アスペクト比の計算
    CalculateAspectRatio();

    // ウィンドウクラスの設定
    wc_ = {};
    wc_.lpfnWndProc = WindowProc;                   // ウィンドウプロシージャ
    wc_.lpszClassName = L"KashipanEngine_Window";   // ウィンドウクラス名
    wc_.hInstance = hInstance_;                     // インスタンスハンドル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);   // カーソル
    wc_.hbrBackground = nullptr;                    // 背景ブラシ（DirectXで描画するためnull）
    wc_.lpszMenuName = nullptr;                     // メニューなし
    wc_.cbClsExtra = 0;                            // 追加のクラスメモリなし
    wc_.cbWndExtra = 0;                            // 追加のウィンドウメモリなし
    wc_.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // アイコン

    // ウィンドウクラスの登録
    if (!RegisterClass(&wc_)) {
        // 既に登録済みの場合は無視
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    // クライアント領域のサイズからウィンドウサイズを計算
    clientRect_ = { 0, 0, clientWidth_, clientHeight_ };
    windowRect_ = clientRect_;
    
    // ウィンドウスタイルに応じてサイズを調整
    AdjustWindowRect(&windowRect_, windowStyle_, FALSE);

    // ウィンドウの作成
    hwnd_ = CreateWindowEx(
        0,                                          // 拡張ウィンドウスタイル
        wc_.lpszClassName,                          // ウィンドウクラス名
        title_.c_str(),                             // ウィンドウタイトル
        windowStyle_,                               // ウィンドウスタイル
        CW_USEDEFAULT,                              // X座標
        CW_USEDEFAULT,                              // Y座標
        windowRect_.right - windowRect_.left,       // ウィンドウ幅
        windowRect_.bottom - windowRect_.top,       // ウィンドウ高さ
        nullptr,                                    // 親ウィンドウ
        nullptr,                                    // メニューハンドル
        hInstance_,                                 // インスタンスハンドル
        this                                        // 作成パラメータ（thisポインタを渡す）
    );

    if (!hwnd_) {
        return false;
    }

    // ウィンドウハンドルにthisポインタを関連付け
    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // ウィンドウを表示
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);

    // 状態の更新
    isVisible_ = true;
    isActive_ = true;

    return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // ウィンドウハンドルからWindowインスタンスを取得
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE:
        // ウィンドウ作成時の処理
        {
            CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lparam);
            Window* window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }
        break;

    case WM_DESTROY:
        // ウィンドウ破棄時の処理
        if (window) {
            window->isVisible_ = false;
            window->isActive_ = false;
        }
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        // ウィンドウを閉じる時の処理
        if (window) {
            window->isVisible_ = false;
        }
        DestroyWindow(hwnd);
        return 0;

    case WM_SIZE:
        // ウィンドウサイズ変更時の処理
        if (window) {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);
            
            // DirectX12用のサイズ変更フラグを設定
            window->isSizing_ = true;
            
            if (wparam == SIZE_MINIMIZED) {
                window->isMinimized_ = true;
                window->isMaximized_ = false;
                window->isVisible_ = false;
            } else if (wparam == SIZE_MAXIMIZED) {
                window->isMinimized_ = false;
                window->isMaximized_ = true;
                window->isVisible_ = true;
                window->clientWidth_ = static_cast<int32_t>(width);
                window->clientHeight_ = static_cast<int32_t>(height);
                window->CalculateAspectRatio();
            } else if (wparam == SIZE_RESTORED) {
                window->isMinimized_ = false;
                window->isMaximized_ = false;
                window->isVisible_ = true;
                window->clientWidth_ = static_cast<int32_t>(width);
                window->clientHeight_ = static_cast<int32_t>(height);
                window->CalculateAspectRatio();
            }
        }
        break;

    case WM_ENTERSIZEMOVE:
        // サイズ変更開始
        if (window) {
            window->isResizing_ = true;
        }
        break;

    case WM_EXITSIZEMOVE:
        // サイズ変更終了
        if (window) {
            window->isResizing_ = false;
            // DirectX12用のサイズ変更フラグも設定
            window->isSizing_ = true;
        }
        break;

    case WM_ACTIVATE:
        // ウィンドウのアクティブ状態変更
        if (window) {
            window->isActive_ = (LOWORD(wparam) != WA_INACTIVE);
        }
        break;

    case WM_GETMINMAXINFO:
        // 最小/最大サイズの制限
        if (window && window->sizeChangeMode_ == SizeChangeMode::FixedAspect) {
            MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lparam);
            // アスペクト比固定の場合の処理
            float aspectRatio = window->aspectRatio_;
            
            // 最小サイズの設定（例：320x240）
            int minWidth = 320;
            int minHeight = static_cast<int>(minWidth / aspectRatio);
            minMaxInfo->ptMinTrackSize.x = minWidth;
            minMaxInfo->ptMinTrackSize.y = minHeight;
            
            // 最大サイズの設定（モニターサイズに基づく）
            RECT workArea;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
            int maxWidth = workArea.right - workArea.left;
            int maxHeight = static_cast<int>(maxWidth / aspectRatio);
            if (maxHeight > workArea.bottom - workArea.top) {
                maxHeight = workArea.bottom - workArea.top;
                maxWidth = static_cast<int>(maxHeight * aspectRatio);
            }
            minMaxInfo->ptMaxTrackSize.x = maxWidth;
            minMaxInfo->ptMaxTrackSize.y = maxHeight;
        }
        break;

    case WM_SIZING:
        // サイズ変更中（アスペクト比固定用）
        if (window && window->sizeChangeMode_ == SizeChangeMode::FixedAspect) {
            RECT* rect = reinterpret_cast<RECT*>(lparam);
            float aspectRatio = window->aspectRatio_;
            
            int width = rect->right - rect->left;
            int height = rect->bottom - rect->top;
            
            // アスペクト比に基づいてサイズを調整
            switch (wparam) {
            case WMSZ_LEFT:
            case WMSZ_RIGHT:
                height = static_cast<int>(width / aspectRatio);
                rect->bottom = rect->top + height;
                break;
            case WMSZ_TOP:
            case WMSZ_BOTTOM:
                width = static_cast<int>(height * aspectRatio);
                rect->right = rect->left + width;
                break;
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
                // 縦横どちらかに合わせて調整
                int newHeight = static_cast<int>(width / aspectRatio);
                if (newHeight != height) {
                    rect->bottom = rect->top + newHeight;
                }
                break;
            }
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool Window::ProcessMessage() {
    MSG msg{};
    
    // メッセージがある場合は処理
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return true;
}

void Window::SetSizeChangeMode(SizeChangeMode sizeChangeMode) {
    sizeChangeMode_ = sizeChangeMode;
    
    // ウィンドウスタイルの更新（必要に応じて）
    if (hwnd_) {
        LONG style = GetWindowLong(hwnd_, GWL_STYLE);
        
        switch (sizeChangeMode) {
        case SizeChangeMode::None:
            // サイズ変更不可
            style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
            break;
        case SizeChangeMode::Normal:
            // 自由変更
            style |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
            break;
        case SizeChangeMode::FixedAspect:
            // アスペクト比固定（サイズ変更は可能）
            style |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
            break;
        }
        
        SetWindowLong(hwnd_, GWL_STYLE, style);
        SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void Window::SetWindowMode(WindowMode windowMode) {
    if (windowMode_ == windowMode || !hwnd_) {
        return;
    }
    
    windowMode_ = windowMode;
    
    switch (windowMode) {
    case WindowMode::Window:
        // ウィンドウモード
        SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
        ShowWindow(hwnd_, SW_NORMAL);
        AdjustWindowSize();
        break;
        
    case WindowMode::FullScreen:
        // フルスクリーンモード
        SetWindowLong(hwnd_, GWL_STYLE, WS_POPUP);
        
        // モニターサイズを取得
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(MonitorFromWindow(hwnd_, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);
        
        SetWindowPos(hwnd_, HWND_TOP,
                     monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                     monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                     monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                     SWP_FRAMECHANGED);
        break;
    }
}

void Window::Cleanup() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    
    if (hInstance_) {
        UnregisterClass(wc_.lpszClassName, hInstance_);
    }
    
    isVisible_ = false;
    isActive_ = false;
}

void Window::CalculateAspectRatio() {
    if (clientHeight_ > 0) {
        aspectRatio_ = static_cast<float>(clientWidth_) / static_cast<float>(clientHeight_);
    } else {
        aspectRatio_ = 1.0f;
    }
}

void Window::AdjustWindowSize() {
    if (!hwnd_) {
        return;
    }
    
    // クライアント領域のサイズからウィンドウサイズを計算
    RECT rect = { 0, 0, clientWidth_, clientHeight_ };
    AdjustWindowRect(&rect, windowStyle_, FALSE);
    
    // ウィンドウサイズを更新
    SetWindowPos(hwnd_, nullptr, 0, 0, 
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOMOVE | SWP_NOZORDER);
}

} // namespace KashipanEngine