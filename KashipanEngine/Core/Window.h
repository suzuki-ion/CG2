#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <string>

namespace KashipanEngine {

/// @brief サイズ変更モード
enum class SizeChangeMode {
    None,           // サイズ変更不可
    Normal,         // 自由変更
    FixedAspect,    // アスペクト比固定
};

/// @brief ウィンドウモード
enum class WindowMode {
    Window,         // ウィンドウ
    FullScreen,     // フルスクリーン
};

/// @brief ウィンドウ用クラス
class Window {
public:
    /// @brief ウィンドウプロシージャ
    /// @param hwnd ウィンドウハンドル
    /// @param msg メッセージ
    /// @param wparam wパラメータ
    /// @param lparam lパラメータ
    /// @return メッセージ処理結果
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    /// @brief コンストラクタ
    /// @param title ウィンドウタイトル
    /// @param width ウィンドウ幅
    /// @param height ウィンドウ高さ
    /// @param windowStyle ウィンドウスタイル
    Window(const std::wstring &title = L"GameWindow",
           int32_t width = 1280, 
           int32_t height = 720, 
           DWORD windowStyle = WS_OVERLAPPEDWINDOW);
    
    virtual ~Window();

    /// @brief メッセージ処理
    /// @return メッセージ処理結果。falseの場合は終了
    bool ProcessMessage();

    /// @brief サイズ変更モードを設定する
    /// @param sizeChangeMode サイズ変更モード
    void SetSizeChangeMode(SizeChangeMode sizeChangeMode);
    
    /// @brief ウィンドウモードを設定する
    /// @param windowMode ウィンドウモード
    void SetWindowMode(WindowMode windowMode);

    /// @brief サイズ変更モードを取得する
    SizeChangeMode GetSizeChangeMode() const noexcept { return sizeChangeMode_; }
    
    /// @brief ウィンドウモードを取得する
    WindowMode GetWindowMode() const noexcept { return windowMode_; }

    /// @brief ウィンドウハンドルを取得する
    HWND GetWindowHandle() const noexcept { return hwnd_; }

    /// @brief ウィンドウクラスを取得する
    const WNDCLASS& GetWindowClass() const noexcept { return wc_; }

    /// @brief クライアント幅を取得する
    int32_t GetClientWidth() const noexcept { return clientWidth_; }

    /// @brief クライアント高さを取得する
    int32_t GetClientHeight() const noexcept { return clientHeight_; }

    /// @brief アスペクト比を取得する
    float GetAspectRatio() const noexcept { return aspectRatio_; }

    /// @brief ウィンドウサイズが変更されたかどうか
    /// @return true:サイズ変更された false:サイズ変更されていない
    bool IsSizing() const noexcept { return isSizing_; }

    /// @brief ウィンドウが表示されているかどうか
    bool IsVisible() const noexcept { return isVisible_; }

    /// @brief ウィンドウがアクティブかどうか
    bool IsActive() const noexcept { return isActive_; }

    /// @brief サイズ変更中かどうか
    bool IsResizing() const noexcept { return isResizing_; }

    /// @brief 最小化されているかどうか
    bool IsMinimized() const noexcept { return isMinimized_; }

    /// @brief 最大化されているかどうか
    bool IsMaximized() const noexcept { return isMaximized_; }

private:
    // ウィンドウ関連
    WNDCLASS wc_{};                     // ウィンドウクラス
    HWND hwnd_{};                       // ウィンドウハンドル
    HINSTANCE hInstance_{};             // インスタンスハンドル
    std::wstring title_;                // ウィンドウタイトル
    DWORD windowStyle_{};               // ウィンドウスタイル
    
    // サイズ関連
    int32_t clientWidth_{};             // クライアント領域の幅
    int32_t clientHeight_{};            // クライアント領域の高さ
    float aspectRatio_{};               // アスペクト比
    RECT windowRect_{};                 // ウィンドウ矩形
    RECT clientRect_{};                 // クライアント矩形
    
    // 状態管理
    SizeChangeMode sizeChangeMode_ = SizeChangeMode::Normal;
    WindowMode windowMode_ = WindowMode::Window;
    bool isVisible_{};                  // ウィンドウが表示されているか
    bool isActive_{};                   // ウィンドウがアクティブか
    bool isResizing_{};                 // サイズ変更中か
    bool isMinimized_{};                // 最小化されているか
    bool isMaximized_{};                // 最大化されているか
    bool isSizing_{};                   // サイズ変更されたか（DirectX12用）

    /// @brief ウィンドウの初期化
    /// @param title ウィンドウタイトル
    /// @param width ウィンドウ幅
    /// @param height ウィンドウ高さ
    /// @param windowStyle ウィンドウスタイル
    /// @return 初期化成功かどうか
    bool InitializeWindow(const std::wstring& title, int32_t width, int32_t height, DWORD windowStyle);

    /// @brief ウィンドウのクリーンアップ
    void Cleanup();

    /// @brief アスペクト比の計算
    void CalculateAspectRatio();

    /// @brief ウィンドウサイズの調整
    void AdjustWindowSize();
};

} // namespace KashipanEngine