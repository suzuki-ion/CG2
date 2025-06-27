#include <cassert>

#include "Input.h"
#include "Base/WinApp.h"
#include "Common/Logs.h"

namespace KashipanEngine {

namespace {

WinApp *sWinApp = nullptr;

/// @brief 初期化済みフラグ
bool sIsInitialized = false;
/// @brief DirectInputインターフェース
IDirectInput8 *sDirectInput = nullptr;
/// @brief キーボードデバイス
IDirectInputDevice8 *sKeyboardDevice = nullptr;
/// @brief マウスデバイス
IDirectInputDevice8 *sMouseDevice = nullptr;

/// @brief キーボードの状態
BYTE sKeyboardState[256] = {};
/// @brief 前回のキーボードの状態
BYTE sPreKeyboardState[256] = {};

/// @brief マウスの状態
DIMOUSESTATE sMouseState = {};
/// @brief 前回のマウスの状態
DIMOUSESTATE sPreMouseState = {};

/// @brief マウスの座標
POINT sMousePos = {};
/// @brief 前回のマウスの座標
POINT sPreMousePos = {};

} // namespace

void Input::Initialize(WinApp *winApp) {
    // 初期化済みフラグをチェック
    if (sIsInitialized) {
        Log("Input is already initialized.", kLogLevelFlagError);
        assert(false);
    }

    sWinApp = winApp;

    // DirectInputの初期化
    HRESULT hr = DirectInput8Create(
        sWinApp->GetWindowClass().hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void **>(&sDirectInput),
        nullptr
    );
    assert(SUCCEEDED(hr));
    
    // キーボードデバイスの初期化
    hr = sDirectInput->CreateDevice(GUID_SysKeyboard, &sKeyboardDevice, nullptr);
    assert(SUCCEEDED(hr));
    // マウスデバイスの初期化
    hr = sDirectInput->CreateDevice(GUID_SysMouse, &sMouseDevice, nullptr);
    assert(SUCCEEDED(hr));
    
    // デバイスの初期化
    hr = sKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(hr));
    hr = sMouseDevice->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(hr));
    // 排他的レベルのセット
    hr = sKeyboardDevice->SetCooperativeLevel(
        sWinApp->GetWindowHandle(),
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
    );
    assert(SUCCEEDED(hr));
    hr = sMouseDevice->SetCooperativeLevel(
        sWinApp->GetWindowHandle(),
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
    );
    assert(SUCCEEDED(hr));
    
    // 初期化完了のログを出力
    LogSimple("Complete Initialize Input.", kLogLevelFlagInfo);
    // 初期化済みフラグを立てる
    sIsInitialized = true;
}

void Input::Finalize() {
    // 初期化済みフラグをチェック
    if (!sIsInitialized) {
        Log("Input is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // 終了処理完了のログを出力
    Log("Complete Finalize Input.", kLogLevelFlagInfo);
}

void Input::Update() {
    // 初期化済みフラグをチェック
    if (!sIsInitialized) {
        Log("Input is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // キーボードの状態を取得
    memcpy(sPreKeyboardState, sKeyboardState, sizeof(sKeyboardState));
    sKeyboardDevice->Acquire();
    sKeyboardDevice->GetDeviceState(sizeof(sKeyboardState), sKeyboardState);

    // マウスの状態を取得
    memcpy(&sPreMouseState, &sMouseState, sizeof(sMouseState));
    sMouseDevice->Acquire();
    sMouseDevice->GetDeviceState(sizeof(sMouseState), &sMouseState);

    // マウスの座標を取得
    sPreMousePos = sMousePos;
    GetCursorPos(&sMousePos);
    ScreenToClient(sWinApp->GetWindowHandle(), &sMousePos);
}

bool Input::IsKeyDown(int key) {
    return (sKeyboardState[key] & 0x80) != 0;
}

bool Input::IsPreKeyDown(int key) {
    return (sPreKeyboardState[key] & 0x80) != 0;
}

bool Input::IsKeyTrigger(int key) {
    return (sKeyboardState[key] & 0x80) != 0 && (sPreKeyboardState[key] & 0x80) == 0;
}

bool Input::IsKeyRelease(int key) {
    return (sKeyboardState[key] & 0x80) == 0 && (sPreKeyboardState[key] & 0x80) != 0;
}

bool Input::IsMouseButtonDown(int button) {
    return (sMouseState.rgbButtons[button] & 0x80) != 0;
}
bool Input::IsPreMouseButtonDown(int button) {
    return (sPreMouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::IsMouseButtonTrigger(int button) {
    return (sMouseState.rgbButtons[button] & 0x80) != 0 && (sPreMouseState.rgbButtons[button] & 0x80) == 0;
}

bool Input::IsMouseButtonRelease(int button) {
    return (sMouseState.rgbButtons[button] & 0x80) == 0 && (sPreMouseState.rgbButtons[button] & 0x80) != 0;
}

int Input::GetMouseX() {
    return static_cast<int>(sMousePos.x);
}

int Input::GetMouseY() {
    return static_cast<int>(sMousePos.y);
}

int Input::GetMouseDeltaX() {
    return static_cast<int>(sMouseState.lX);
}

int Input::GetMouseDeltaY() {
    return static_cast<int>(sMouseState.lY);
}

int Input::GetMouseWheel() {
    return static_cast<int>(sMouseState.lZ);
}

} // namespace KashipanEngine