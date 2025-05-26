#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

namespace KashipanEngine {

enum SizeChangeMode {
    kNone,          //!< サイズ変更不可
    kNormal,        //!< 自由変更
    kFixedAspect,   //!< アスペクト比固定
};

/// @brief Windowsアプリクラス
class WinApp final {
public:
    WinApp(const std::wstring &title, UINT windowStyle, int32_t width, int32_t height);
    ~WinApp();

    /// @brief ウィンドウクラス取得
    /// @return ウィンドウクラス
    WNDCLASS GetWindowClass() const { return wc_; }

    /// @brief ウィンドウハンドル取得
    /// @return ウィンドウハンドル
    HWND GetWindowHandle() const { return hwnd_; }

    /// @brief クライアントサイズの横幅取得
    /// @return クライアントサイズの横幅
    int32_t GetClientWidth() const;

    /// @brief クライアントサイズの縦幅取得
    /// @return クライアントサイズの縦幅
    int32_t GetClientHeight() const;

    /// @brief サイズ変更モードの取得
    /// @return サイズ変更モード
    SizeChangeMode GetSizeChangeMode() const { return sizeChangeMode_; }

    /// @brief ウィンドウがサイズ変更されたかどうか
    /// @return true:サイズ変更された false:サイズ変更されていない
    bool IsSizing() { return isSizing_; }

    /// @brief サイズ変更モードの設定
    /// @param mode サイズ変更モード
    void SetSizeChangeMode(SizeChangeMode mode);

    /// @brief メッセージ処理
    /// @return メッセージ処理結果
    int ProccessMessage();

private:
    /// @brief ウィンドウクラス
    WNDCLASS wc_{};
    /// @brief ウィンドウハンドル
    HWND hwnd_{};
    /// @brief クライアントサイズの横幅
    int32_t clientWidth_;
    /// @brief クライアントサイズの縦幅
    int32_t clientHeight_;
    /// @brief ウィンドウサイズを表す構造体
    RECT wrc_ = {};
    /// @brief メッセージ
    MSG msg_ = {};
    /// @brief ウィンドウスタイル
    UINT windowStyle_;
    /// @brief サイズ変更モード
    SizeChangeMode sizeChangeMode_ = SizeChangeMode::kNone;
    /// @brief アスペクト比
    float aspectRatio_;
    /// @brief サイズ変更フラグ
    bool isSizing_ = false;

    /// @brief ウィンドウプロシージャ
    /// @param hwnd ウィンドウハンドル
    /// @param msg メッセージ
    /// @param wparam 
    /// @param lparam 
    /// @return メッセージ処理結果
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

} // namespace KashipanEngine