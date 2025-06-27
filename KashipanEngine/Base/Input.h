#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace KashipanEngine{

// 前方宣言
class WinApp;

// 入力はどこでも使えるようにしたいのでstaticにする

/// @brief 入力管理クラス
class Input {
public:
    Input() = delete;
    Input(const Input &) = delete;
    Input(Input &&) = delete;
    Input &operator=(const Input &) = delete;

    /// @brief 初期化処理
    /// @param winApp WinAppインスタンス
    static void Initialize(WinApp *winApp);

    /// @brief 終了処理
    static void Finalize();

    /// @brief 入力状態更新
    static void Update();

    /// @brief キーの押下状態を取得
    /// @param key キーコード
    /// @return 押下状態
    static bool IsKeyDown(int key);

    /// @brief 前回のキーの押下状態を取得
    /// @param key キーコード
    /// @return 前回の押下状態
    static bool IsPreKeyDown(int key);

    /// @brief キーのトリガー状態を取得
    /// @param key キーコード
    /// @return トリガー状態
    static bool IsKeyTrigger(int key);

    /// @brief キーのリリース状態を取得
    /// @param key キーコード
    /// @return リリース状態
    static bool IsKeyRelease(int key);

    /// @brief マウスのボタンの押下状態を取得
    /// @param button マウスボタンコード
    /// @return 押下状態
    static bool IsMouseButtonDown(int button);

    /// @brief 前回のマウスのボタンの押下状態を取得
    /// @param button マウスボタンコード
    /// @return 前回の押下状態
    static bool IsPreMouseButtonDown(int button);

    /// @brief マウスのボタンのトリガー状態を取得
    /// @param button マウスボタンコード
    /// @return トリガー状態
    static bool IsMouseButtonTrigger(int button);

    /// @brief マウスのボタンのリリース状態を取得
    /// @param button マウスボタンコード
    /// @return リリース状態
    static bool IsMouseButtonRelease(int button);

    /// @brief マウスカーソルのX座標を取得
    /// @return マウスカーソルのX座標
    static int GetMouseX();

    /// @brief マウスカーソルのY座標を取得
    /// @return マウスカーソルのY座標
    static int GetMouseY();

    /// @brief マウスカーソルのX座標の移動量を取得
    /// @return マウスカーソルのX座標の移動量
    static int GetMouseDeltaX();

    /// @brief マウスカーソルのY座標の移動量を取得
    /// @return マウスカーソルのY座標の移動量
    static int GetMouseDeltaY();

    /// @brief マウスホイールの回転量を取得
    /// @return マウスホイールの回転量
    static int GetMouseWheel();

private:
};

} // namespace KashipanEngine