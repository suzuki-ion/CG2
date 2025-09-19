#pragma once
#include <Windows.h>
#include <d3d12.h>
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
    Window() = default;
    virtual ~Window() = default;

    /// @brief サイズ変更モードを設定する
    /// @param sizeChangeMode サイズ変更モード
    void SetSizeChangeMode(SizeChangeMode sizeChangeMode);
    /// @brief ウィンドウモードを設定する
    /// @param windowMode ウィンドウモード
    void SetWindowMode(WindowMode windowMode);
    
    /// @brief サイズ変更モードを取得する
    [[nodiscard]] SizeChangeMode GetSizeChangeMode() const noexcept {
        return sizeChangeMode_;
    }
    /// @brief ウィンドウモードを取得する
    [[nodiscard]] WindowMode GetWindowMode() const noexcept {
        return windowMode_;
    }

private:
    SizeChangeMode sizeChangeMode_ = SizeChangeMode::Normal;
    WindowMode windowMode_ = WindowMode::Window;
};

} // namespace KashipanEngine