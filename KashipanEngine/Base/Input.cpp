#include <cassert>
#include <unordered_map>
#include <functional>

#include "Input.h"
#include "Base/WinApp.h"
#include "Common/Logs.h"

#pragma comment(lib, "xinput.lib")

namespace KashipanEngine {

namespace {

/// @brief WinAppへのポインタ
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
/// @brief 前回のマウスの差分座標
POINT sPreMouseDeltaPos = {};

/// @brief コントローラーの状態
XINPUT_STATE sControllerState[4] = {};
/// @brief 前回のコントローラーの状態
XINPUT_STATE sPreControllerState[4] = {};

/// @brief コントローラーのスティックのデッドゾーン
SHORT sControllerStickDeadZone = 4096;

/// @brief コントローラーの振動状態
XINPUT_VIBRATION vibration = { 0 };

/// @brief キーの押下取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::DownStateOption, std::function<bool(int)>>> sGetKeyFunctions = {
    { Input::CurrentOption::Current, {
        { Input::DownStateOption::Down, Input::IsKeyDown },
        { Input::DownStateOption::Trigger, Input::IsKeyTrigger },
        { Input::DownStateOption::Release, Input::IsKeyRelease }
    }},
    { Input::CurrentOption::Previous, {
        { Input::DownStateOption::Down, Input::IsPreKeyDown },
        { Input::DownStateOption::Trigger, Input::IsKeyTrigger },
        { Input::DownStateOption::Release, Input::IsKeyRelease }
    }}
};

/// @brief マウスボタンの押下取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::DownStateOption, std::function<bool(int)>>> sGetMouseButtonFunctions = {
    { Input::CurrentOption::Current, {
        { Input::DownStateOption::Down, Input::IsMouseButtonDown },
        { Input::DownStateOption::Trigger, Input::IsMouseButtonTrigger },
        { Input::DownStateOption::Release, Input::IsMouseButtonRelease }
    }},
    { Input::CurrentOption::Previous, {
        { Input::DownStateOption::Down, Input::IsPreMouseButtonDown },
        { Input::DownStateOption::Trigger, Input::IsMouseButtonTrigger },
        { Input::DownStateOption::Release, Input::IsMouseButtonRelease }
    }}
};

/// @brief マウスカーソル(+マウスホイール)の位置取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::AxisOption, std::unordered_map<Input::ValueOption, std::function<int()>>>> sGetMousePositionFunctions = {
    { Input::CurrentOption::Current, {
        { Input::AxisOption::X, {
            { Input::ValueOption::Actual, Input::GetMouseX },
            { Input::ValueOption::Delta, Input::GetMouseDeltaX }
        }},
        { Input::AxisOption::Y, {
            { Input::ValueOption::Actual, Input::GetMouseY },
            { Input::ValueOption::Delta, Input::GetMouseDeltaY }
        }},
        { Input::AxisOption::Z, {
            { Input::ValueOption::Actual, Input::GetMouseWheel },
            { Input::ValueOption::Delta, Input::GetMouseWheel }
        }}
    }},
    { Input::CurrentOption::Previous, {
        { Input::AxisOption::X, {
            { Input::ValueOption::Actual, Input::GetPreMouseX },
            { Input::ValueOption::Delta, Input::GetPreMouseDeltaX }
        }},
        { Input::AxisOption::Y, {
            { Input::ValueOption::Actual, Input::GetPreMouseY },
            { Input::ValueOption::Delta, Input::GetPreMouseDeltaY }
        }},
        { Input::AxisOption::Z, {
            { Input::ValueOption::Actual, Input::GetPreMouseWheel },
            { Input::ValueOption::Delta, Input::GetPreMouseWheel }
        }}
    }}
};

/// @brief コントローラーのトリガー取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::LeftRightOption, std::unordered_map<Input::ValueOption, std::function<int(int)>>>> sGetXBoxTriggerFunctions = {
    { Input::CurrentOption::Current, {
        { Input::LeftRightOption::Left, {
            { Input::ValueOption::Actual, Input::GetXBoxLeftTrigger },
            { Input::ValueOption::Delta, Input::GetXBoxLeftTriggerDelta }
        }},
        { Input::LeftRightOption::Right, {
            { Input::ValueOption::Actual, Input::GetXBoxRightTrigger },
            { Input::ValueOption::Delta, Input::GetXBoxRightTriggerDelta }
        }}
    }},
    { Input::CurrentOption::Previous, {
        { Input::LeftRightOption::Left, {
            { Input::ValueOption::Actual, Input::GetPreXBoxLeftTrigger },
            { Input::ValueOption::Delta, Input::GetPreXBoxLeftTriggerDelta }
        }},
        { Input::LeftRightOption::Right, {
            { Input::ValueOption::Actual, Input::GetPreXBoxRightTrigger },
            { Input::ValueOption::Delta, Input::GetPreXBoxRightTriggerDelta }
        }}
    }}
};

/// @brief コントローラーのトリガー比率取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::LeftRightOption, std::unordered_map<Input::ValueOption, std::function<float(int)>>>> sGetXBoxTriggerRatioFunctions = {
    { Input::CurrentOption::Current, {
        { Input::LeftRightOption::Left, {
            { Input::ValueOption::Actual, Input::GetXBoxLeftTriggerRatio },
            { Input::ValueOption::Delta, Input::GetXBoxLeftTriggerDeltaRatio }
        }},
        { Input::LeftRightOption::Right, {
            { Input::ValueOption::Actual, Input::GetXBoxRightTriggerRatio },
            { Input::ValueOption::Delta, Input::GetXBoxRightTriggerDeltaRatio }
        }}
    }},
    { Input::CurrentOption::Previous, {
        { Input::LeftRightOption::Left, {
            { Input::ValueOption::Actual, Input::GetPreXBoxLeftTriggerRatio },
            { Input::ValueOption::Delta, Input::GetPreXBoxLeftTriggerDeltaRatio }
        }},
        { Input::LeftRightOption::Right, {
            { Input::ValueOption::Actual, Input::GetPreXBoxRightTriggerRatio },
            { Input::ValueOption::Delta, Input::GetPreXBoxRightTriggerDeltaRatio }
        }}
    }}
};

/// @brief コントローラーのスティック取得関数マップ
std::unordered_map < Input::CurrentOption, std::unordered_map<Input::LeftRightOption, std::unordered_map<Input::AxisOption, std::unordered_map<Input::ValueOption, std::function<int(int)>>>>> sGetXBoxStickFunctions = {
    { Input::CurrentOption::Current, {
        { Input::LeftRightOption::Left, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetXBoxLeftStickX },
                { Input::ValueOption::Delta, Input::GetXBoxLeftStickDeltaX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetXBoxLeftStickY },
                { Input::ValueOption::Delta, Input::GetXBoxLeftStickDeltaY }
            }}
        }},
        { Input::LeftRightOption::Right, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetXBoxRightStickX },
                { Input::ValueOption::Delta, Input::GetXBoxRightStickDeltaX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetXBoxRightStickY },
                { Input::ValueOption::Delta, Input::GetXBoxRightStickDeltaY }
            }}
        }}
    }},
    { Input::CurrentOption::Previous, {
        { Input::LeftRightOption::Left, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetPreXBoxLeftStickX },
                { Input::ValueOption::Delta, Input::GetPreXBoxLeftStickDeltaX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetPreXBoxLeftStickY },
                { Input::ValueOption::Delta, Input::GetPreXBoxLeftStickDeltaY }
            }}
        }},
        { Input::LeftRightOption::Right, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetPreXBoxRightStickX },
                { Input::ValueOption::Delta, Input::GetPreXBoxRightStickDeltaX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetPreXBoxRightStickY },
                { Input::ValueOption::Delta, Input::GetPreXBoxRightStickDeltaY }
            }}
        }}
    }}
};

/// @brief コントローラーのスティック比率取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::LeftRightOption, std::unordered_map<Input::AxisOption, std::unordered_map<Input::ValueOption, std::function<float(int)>>>>> sGetXBoxStickRatioFunctions = {
    { Input::CurrentOption::Current, {
        { Input::LeftRightOption::Left, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetXBoxLeftStickRatioX },
                { Input::ValueOption::Delta, Input::GetXBoxLeftStickDeltaRatioX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetXBoxLeftStickRatioY },
                { Input::ValueOption::Delta, Input::GetXBoxLeftStickDeltaRatioY }
            }}
        }},
        { Input::LeftRightOption::Right, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetXBoxRightStickRatioX },
                { Input::ValueOption::Delta, Input::GetXBoxRightStickDeltaRatioX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetXBoxRightStickRatioY },
                { Input::ValueOption::Delta, Input::GetXBoxRightStickDeltaRatioY }
            }}
        }}
    }},
    { Input::CurrentOption::Previous, {
        { Input::LeftRightOption::Left, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetPreXBoxLeftStickRatioX },
                { Input::ValueOption::Delta, Input::GetPreXBoxLeftStickDeltaRatioX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetPreXBoxLeftStickRatioY },
                { Input::ValueOption::Delta, Input::GetPreXBoxLeftStickDeltaRatioY }
            }}
        }},
        { Input::LeftRightOption::Right, {
            { Input::AxisOption::X, {
                { Input::ValueOption::Actual, Input::GetPreXBoxRightStickRatioX },
                { Input::ValueOption::Delta, Input::GetPreXBoxRightStickDeltaRatioX }
            }},
            { Input::AxisOption::Y, {
                { Input::ValueOption::Actual, Input::GetPreXBoxRightStickRatioY },
                { Input::ValueOption::Delta, Input::GetPreXBoxRightStickDeltaRatioY }
            }}
        }}
    }}
};

/// @brief コントローラーのボタン取得関数マップ
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::DownStateOption, std::function<bool(XBoxButtonCode, int)>>> sGetXBoxButtonFunctions = {
    { Input::CurrentOption::Current, {
        { Input::DownStateOption::Down, Input::IsXBoxButtonDown },
        { Input::DownStateOption::Trigger, Input::IsXBoxButtonTrigger },
        { Input::DownStateOption::Release, Input::IsXBoxButtonRelease }
    }},
    { Input::CurrentOption::Previous, {
        { Input::DownStateOption::Down, Input::IsPreXBoxButtonDown },
        { Input::DownStateOption::Trigger, Input::IsXBoxButtonTrigger },
        { Input::DownStateOption::Release, Input::IsXBoxButtonRelease }
    }}
};

} // namespace

void Input::Initialize(WinApp *winApp) {
    // 初期化済みフラグをチェック
    if (sIsInitialized) {
        Log("Input is already initialized.", kLogLevelFlagError);
        assert(false);
    }

    sWinApp = winApp;

    //==================================================
    // DirectInputの初期化
    //==================================================

    HRESULT hr = DirectInput8Create(
        sWinApp->GetWindowClass().hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void **>(&sDirectInput),
        nullptr
    );
    assert(SUCCEEDED(hr));
    
    //==================================================
    // キーボードのデバイスの初期化
    //==================================================

    hr = sDirectInput->CreateDevice(GUID_SysKeyboard, &sKeyboardDevice, nullptr);
    assert(SUCCEEDED(hr));
    hr = sKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(hr));

    //==================================================
    // マウスのデバイスの初期化
    //==================================================

    hr = sDirectInput->CreateDevice(GUID_SysMouse, &sMouseDevice, nullptr);
    assert(SUCCEEDED(hr));    
    hr = sMouseDevice->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(hr));

    //==================================================
    // コントローラーの状態の初期化
    //==================================================

    for (int i = 0; i < 4; ++i) {
        ZeroMemory(&sControllerState[i], sizeof(XINPUT_STATE));
        ZeroMemory(&sPreControllerState[i], sizeof(XINPUT_STATE));
    }
    
    //==================================================
    // 排他的レベルの設定
    //==================================================

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

    // コントローラーの状態を取得
    for (int i = 0; i < 4; ++i) {
        memcpy(&sPreControllerState[i], &sControllerState[i], sizeof(XINPUT_STATE));
        ZeroMemory(&sControllerState[i], sizeof(XINPUT_STATE));
        DWORD dw = XInputGetState(i, &sControllerState[i]);
        // コントローラーが接続されていない場合は状態をクリア
        if (dw == ERROR_DEVICE_NOT_CONNECTED) {
            ZeroMemory(&sControllerState[i], sizeof(XINPUT_STATE));
            continue;
        }

        // スティックの値がデッドゾーン以下の場合は0に設定
        if (sControllerState[i].Gamepad.sThumbLX < +sControllerStickDeadZone &&
            sControllerState[i].Gamepad.sThumbLX > -sControllerStickDeadZone) {
            sControllerState[i].Gamepad.sThumbLX = 0;
        }
        if (sControllerState[i].Gamepad.sThumbLY < +sControllerStickDeadZone &&
            sControllerState[i].Gamepad.sThumbLY > -sControllerStickDeadZone) {
            sControllerState[i].Gamepad.sThumbLY = 0;
        }
        if (sControllerState[i].Gamepad.sThumbRX < +sControllerStickDeadZone &&
            sControllerState[i].Gamepad.sThumbRX > -sControllerStickDeadZone) {
            sControllerState[i].Gamepad.sThumbRX = 0;
        }
        if (sControllerState[i].Gamepad.sThumbRY < +sControllerStickDeadZone &&
            sControllerState[i].Gamepad.sThumbRY > -sControllerStickDeadZone) {
            sControllerState[i].Gamepad.sThumbRY = 0;
        }
    }
    
    // マウスの座標を取得
    sPreMousePos = sMousePos;
    GetCursorPos(&sMousePos);
    ScreenToClient(sWinApp->GetWindowHandle(), &sMousePos);
}

bool Input::IsKey(CurrentOption currentOption, DownStateOption downStateOption, int key) {
    // キーの押下状態を取得する関数を呼び出す
    auto itCurrent = sGetKeyFunctions.find(currentOption);
    if (itCurrent == sGetKeyFunctions.end()) {
        return false;
    }
    auto itDownState = itCurrent->second.find(downStateOption);
    if (itDownState == itCurrent->second.end()) {
        return false;
    }
    auto getKeyFunction = itDownState->second;
    if (getKeyFunction) {
        return getKeyFunction(key);
    }
    
    return false;
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

bool Input::IsMouseButton(CurrentOption currentOption, DownStateOption downStateOption, int button) {
    // マウスボタンの押下状態を取得する関数を呼び出す
    auto itCurrent = sGetMouseButtonFunctions.find(currentOption);
    if (itCurrent == sGetMouseButtonFunctions.end()) {
        return false;
    }
    auto itDownState = itCurrent->second.find(downStateOption);
    if (itDownState == itCurrent->second.end()) {
        return false;
    }
    auto getMouseButtonFunction = itDownState->second;
    if (getMouseButtonFunction) {
        return getMouseButtonFunction(button);
    }
    
    return false;
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

int Input::GetMousePos(CurrentOption currentOption, AxisOption axisOption, ValueOption valueOption) {
    // マウスの位置を取得する関数を呼び出す
    auto itCurrent = sGetMousePositionFunctions.find(currentOption);
    if (itCurrent == sGetMousePositionFunctions.end()) {
        return 0;
    }
    auto itAxis = itCurrent->second.find(axisOption);
    if (itAxis == itCurrent->second.end()) {
        return 0;
    }
    auto itValue = itAxis->second.find(valueOption);
    if (itValue == itAxis->second.end()) {
        return 0;
    }
    auto getMousePositionFunction = itValue->second;
    if (getMousePositionFunction) {
        return getMousePositionFunction();
    }
    
    return 0;
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

int Input::GetPreMouseX() {
    return static_cast<int>(sPreMousePos.x);
}

int Input::GetPreMouseY() {
    return static_cast<int>(sPreMousePos.y);
}

int Input::GetPreMouseDeltaX() {
    return static_cast<int>(sPreMouseState.lX);
}

int Input::GetPreMouseDeltaY() {
    return static_cast<int>(sPreMouseState.lY);
}

int Input::GetMouseWheel() {
    return static_cast<int>(sMouseState.lZ);
}

int Input::GetPreMouseWheel() {
    return static_cast<int>(sPreMouseState.lZ);
}

int Input::GetXBoxTrigger(CurrentOption currentOption, LeftRightOption leftRightOption, ValueOption valueOption, int index) {
    return 0;
}

float Input::GetXBoxTriggerRatio(CurrentOption currentOption, LeftRightOption leftRightOption, ValueOption valueOption, int index) {
    return 0.0f;
}

int Input::GetXBoxLeftTrigger(int index) {
    return sControllerState[index].Gamepad.bLeftTrigger;
}

int Input::GetXBoxRightTrigger(int index) {
    return sControllerState[index].Gamepad.bRightTrigger;
}

int Input::GetXBoxLeftTriggerDelta(int index) {
    return 0;
}

int Input::GetXBoxRightTriggerDelta(int index) {
    return 0;
}

float Input::GetXBoxLeftTriggerRatio(int index) {
    return 0.0f;
}

float Input::GetXBoxRightTriggerRatio(int index) {
    return 0.0f;
}

float Input::GetXBoxLeftTriggerDeltaRatio(int index) {
    return 0.0f;
}

float Input::GetXBoxRightTriggerDeltaRatio(int index) {
    return 0.0f;
}

int Input::GetXBoxStick(CurrentOption currentOption, LeftRightOption leftRightOption, AxisOption axisOption, ValueOption valueOption, int index) {
    return 0;
}

int Input::GetPreXBoxLeftTrigger(int index) {
    return 0;
}

int Input::GetPreXBoxRightTrigger(int index) {
    return 0;
}

int Input::GetPreXBoxLeftTriggerDelta(int index) {
    return 0;
}

int Input::GetPreXBoxRightTriggerDelta(int index) {
    return 0;
}

float Input::GetPreXBoxLeftTriggerRatio(int index) {
    return 0.0f;
}

float Input::GetPreXBoxRightTriggerRatio(int index) {
    return 0.0f;
}

float Input::GetPreXBoxLeftTriggerDeltaRatio(int index) {
    return 0.0f;
}

float Input::GetPreXBoxRightTriggerDeltaRatio(int index) {
    return 0.0f;
}

float Input::GetXBoxStickRatio(CurrentOption currentOption, LeftRightOption leftRightOption, AxisOption axisOption, ValueOption valueOption, int index) {
    return 0.0f;
}

int Input::GetXBoxLeftStickX(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbLX);
}

int Input::GetXBoxLeftStickY(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbLY);
}

int Input::GetXBoxLeftStickDeltaX(int index) {
    return 0;
}

int Input::GetXBoxLeftStickDeltaY(int index) {
    return 0;
}

float Input::GetXBoxLeftStickRatioX(int index) {
    return 0.0f;
}

float Input::GetXBoxLeftStickRatioY(int index) {
    return 0.0f;
}

float Input::GetXBoxLeftStickDeltaRatioX(int index) {
    return 0.0f;
}

float Input::GetXBoxLeftStickDeltaRatioY(int index) {
    return 0.0f;
}

int Input::GetXBoxRightStickX(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbRX);
}

int Input::GetXBoxRightStickY(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbRY);
}

int Input::GetXBoxRightStickDeltaX(int index) {
    return 0;
}

int Input::GetXBoxRightStickDeltaY(int index) {
    return 0;
}

float Input::GetXBoxRightStickRatioX(int index) {
    return 0.0f;
}

float Input::GetXBoxRightStickRatioY(int index) {
    return 0.0f;
}

float Input::GetXBoxRightStickDeltaRatioX(int index) {
    return 0.0f;
}

float Input::GetXBoxRightStickDeltaRatioY(int index) {
    return 0.0f;
}

int Input::GetPreXBoxLeftStickX(int index) {
    return 0;
}

int Input::GetPreXBoxLeftStickY(int index) {
    return 0;
}

int Input::GetPreXBoxLeftStickDeltaX(int index) {
    return 0;
}

int Input::GetPreXBoxLeftStickDeltaY(int index) {
    return 0;
}

float Input::GetPreXBoxLeftStickRatioX(int index) {
    return 0.0f;
}

float Input::GetPreXBoxLeftStickRatioY(int index) {
    return 0.0f;
}

float Input::GetPreXBoxLeftStickDeltaRatioX(int index) {
    return 0.0f;
}

float Input::GetPreXBoxLeftStickDeltaRatioY(int index) {
    return 0.0f;
}

int Input::GetPreXBoxRightStickX(int index) {
    return 0;
}

int Input::GetPreXBoxRightStickY(int index) {
    return 0;
}

int Input::GetPreXBoxRightStickDeltaX(int index) {
    return 0;
}

int Input::GetPreXBoxRightStickDeltaY(int index) {
    return 0;
}

float Input::GetPreXBoxRightStickRatioX(int index) {
    return 0.0f;
}

float Input::GetPreXBoxRightStickRatioY(int index) {
    return 0.0f;
}

float Input::GetPreXBoxRightStickDeltaRatioX(int index) {
    return 0.0f;
}

float Input::GetPreXBoxRightStickDeltaRatioY(int index) {
    return 0.0f;
}

bool Input::IsXBoxButton(CurrentOption currentOption, DownStateOption downStateOption, XBoxButtonCode button, int index) {
    return false;
}

bool Input::IsXBoxButtonDown(XBoxButtonCode button, int index) {
    return sControllerState[index].Gamepad.wButtons & static_cast<WORD>(button) ? true : false;
}

bool Input::IsPreXBoxButtonDown(XBoxButtonCode button, int index) {
    return false;
}

bool Input::IsXBoxButtonTrigger(XBoxButtonCode button, int index) {
    return false;
}

bool Input::IsXBoxButtonRelease(XBoxButtonCode button, int index) {
    return false;
}

int Input::GetXBoxConnectedCount() {
    return 0;
}

bool Input::IsXBoxConnected(int index) {
    return false;
}

bool Input::IsPreXBoxConnected(int index) {
    return false;
}

void Input::SetXBoxVibration(int index, int leftMotor, int rightMotor) {
    // 値が-1でなければ振動を設定
    if (leftMotor > -1) {
        vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor);
    }
    if (rightMotor > -1) {
        vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor);
    }
    // コントローラーの振動を設定
    XInputSetState(index, &vibration);
}

void Input::StopXBoxVibration(int index) {
    vibration.wLeftMotorSpeed = 0;
    vibration.wRightMotorSpeed = 0;
    XInputSetState(index, &vibration);
}

int Input::GetXBoxVibration(int index, LeftRightOption leftRightOption) {
    switch (leftRightOption) {
        case Input::LeftRightOption::Left:
            return vibration.wLeftMotorSpeed;
            break;
        case Input::LeftRightOption::Right:
            return vibration.wRightMotorSpeed;
            break;
    }
    return 0;
}

} // namespace KashipanEngine