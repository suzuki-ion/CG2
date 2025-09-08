#include <cassert>
#include <unordered_map>
#include <functional>
#include <algorithm>

#include "Input.h"
#include "Base/WinApp.h"
#include "Base/ScreenBuffer.h"
#include "Common/Logs.h"
#include "Math/Vector2.h"

#pragma comment(lib, "xinput.lib")

namespace KashipanEngine {

namespace {

/// @brief WinAppへのポインタ
WinApp *sWinApp = nullptr;
/// @brief メインのスクリーンへのポインタ
ScreenBuffer *sMainScreen = nullptr;

/// @brief 初期化済みフラグ
bool sIsInitialized = false;

/// @brief DirectInputインターフェース
IDirectInput8 *sDirectInput = nullptr;
/// @brief キーボードデバイス
IDirectInputDevice8 *sKeyboardDevice = nullptr;
/// @brief マウスデバイス
IDirectInputDevice8 *sMouseDevice = nullptr;

//==================================================
// キーボード
//==================================================

/// @brief キーボードの状態
BYTE sKeyboardState[256] = {};
/// @brief 前回のキーボードの状態
BYTE sPreKeyboardState[256] = {};
/// @brief キーコードのマップ
const std::unordered_map<std::string, int> kDikMap = {
    {"DIK_ESCAPE", DIK_ESCAPE},
    {"DIK_1", DIK_1},
    {"DIK_2", DIK_2},
    {"DIK_3", DIK_3},
    {"DIK_4", DIK_4},
    {"DIK_5", DIK_5},
    {"DIK_6", DIK_6},
    {"DIK_7", DIK_7},
    {"DIK_8", DIK_8},
    {"DIK_9", DIK_9},
    {"DIK_0", DIK_0},
    {"DIK_MINUS", DIK_MINUS},
    {"DIK_EQUALS", DIK_EQUALS},
    {"DIK_BACK", DIK_BACK},
    {"DIK_TAB", DIK_TAB},
    {"DIK_Q", DIK_Q},
    {"DIK_W", DIK_W},
    {"DIK_E", DIK_E},
    {"DIK_R", DIK_R},
    {"DIK_T", DIK_T},
    {"DIK_Y", DIK_Y},
    {"DIK_U", DIK_U},
    {"DIK_I", DIK_I},
    {"DIK_O", DIK_O},
    {"DIK_P", DIK_P},
    {"DIK_LBRACKET", DIK_LBRACKET},
    {"DIK_RBRACKET", DIK_RBRACKET},
    {"DIK_RETURN", DIK_RETURN},
    {"DIK_LCONTROL", DIK_LCONTROL},
    {"DIK_A", DIK_A},
    {"DIK_S", DIK_S},
    {"DIK_D", DIK_D},
    {"DIK_F", DIK_F},
    {"DIK_G", DIK_G},
    {"DIK_H", DIK_H},
    {"DIK_J", DIK_J},
    {"DIK_K", DIK_K},
    {"DIK_L", DIK_L},
    {"DIK_SEMICOLON", DIK_SEMICOLON},
    {"DIK_APOSTROPHE", DIK_APOSTROPHE},
    {"DIK_GRAVE", DIK_GRAVE},
    {"DIK_LSHIFT", DIK_LSHIFT},
    {"DIK_BACKSLASH", DIK_BACKSLASH},
    {"DIK_Z", DIK_Z},
    {"DIK_X", DIK_X},
    {"DIK_C", DIK_C},
    {"DIK_V", DIK_V},
    {"DIK_B", DIK_B},
    {"DIK_N", DIK_N},
    {"DIK_M", DIK_M},
    {"DIK_COMMA", DIK_COMMA},
    {"DIK_PERIOD", DIK_PERIOD},
    {"DIK_SLASH", DIK_SLASH},
    {"DIK_RSHIFT", DIK_RSHIFT},
    {"DIK_MULTIPLY", DIK_MULTIPLY},
    {"DIK_LMENU", DIK_LMENU},
    {"DIK_SPACE", DIK_SPACE},
    {"DIK_CAPITAL", DIK_CAPITAL},
    {"DIK_F1", DIK_F1},
    {"DIK_F2", DIK_F2},
    {"DIK_F3", DIK_F3},
    {"DIK_F4", DIK_F4},
    {"DIK_F5", DIK_F5},
    {"DIK_F6", DIK_F6},
    {"DIK_F7", DIK_F7},
    {"DIK_F8", DIK_F8},
    {"DIK_F9", DIK_F9},
    {"DIK_F10", DIK_F10},
    {"DIK_NUMLOCK", DIK_NUMLOCK},
    {"DIK_SCROLL", DIK_SCROLL},
    {"DIK_NUMPAD7", DIK_NUMPAD7},
    {"DIK_NUMPAD8", DIK_NUMPAD8},
    {"DIK_NUMPAD9", DIK_NUMPAD9},
    {"DIK_SUBTRACT", DIK_SUBTRACT},
    {"DIK_NUMPAD4", DIK_NUMPAD4},
    {"DIK_NUMPAD5", DIK_NUMPAD5},
    {"DIK_NUMPAD6", DIK_NUMPAD6},
    {"DIK_ADD", DIK_ADD},
    {"DIK_NUMPAD1", DIK_NUMPAD1},
    {"DIK_NUMPAD2", DIK_NUMPAD2},
    {"DIK_NUMPAD3", DIK_NUMPAD3},
    {"DIK_NUMPAD0", DIK_NUMPAD0},
    {"DIK_DECIMAL", DIK_DECIMAL},
    {"DIK_OEM_102", DIK_OEM_102},
    {"DIK_F11", DIK_F11},
    {"DIK_F12", DIK_F12},
    {"DIK_F13", DIK_F13},
    {"DIK_F14", DIK_F14},
    {"DIK_F15", DIK_F15},
    {"DIK_KANA", DIK_KANA},
    {"DIK_ABNT_C1", DIK_ABNT_C1},
    {"DIK_CONVERT", DIK_CONVERT},
    {"DIK_NOCONVERT", DIK_NOCONVERT},
    {"DIK_YEN", DIK_YEN},
    {"DIK_ABNT_C2", DIK_ABNT_C2},
    {"DIK_NUMPADEQUALS", DIK_NUMPADEQUALS},
    {"DIK_PREVTRACK", DIK_PREVTRACK},
    {"DIK_AT", DIK_AT},
    {"DIK_COLON", DIK_COLON},
    {"DIK_UNDERLINE", DIK_UNDERLINE},
    {"DIK_KANJI", DIK_KANJI},
    {"DIK_STOP", DIK_STOP},
    {"DIK_AX", DIK_AX},
    {"DIK_UNLABELED", DIK_UNLABELED},
    {"DIK_NEXTTRACK", DIK_NEXTTRACK},
    {"DIK_NUMPADENTER", DIK_NUMPADENTER},
    {"DIK_RCONTROL", DIK_RCONTROL},
    {"DIK_MUTE", DIK_MUTE},
    {"DIK_CALCULATOR", DIK_CALCULATOR},
    {"DIK_PLAYPAUSE", DIK_PLAYPAUSE},
    {"DIK_MEDIASTOP", DIK_MEDIASTOP},
    {"DIK_VOLUMEDOWN", DIK_VOLUMEDOWN},
    {"DIK_VOLUMEUP", DIK_VOLUMEUP},
    {"DIK_WEBHOME", DIK_WEBHOME},
    {"DIK_NUMPADCOMMA", DIK_NUMPADCOMMA},
    {"DIK_DIVIDE", DIK_DIVIDE},
    {"DIK_SYSRQ", DIK_SYSRQ},
    {"DIK_RMENU", DIK_RMENU},
    {"DIK_PAUSE", DIK_PAUSE},
    {"DIK_HOME", DIK_HOME},
    {"DIK_UP", DIK_UP},
    {"DIK_PRIOR", DIK_PRIOR},
    {"DIK_LEFT", DIK_LEFT},
    {"DIK_RIGHT", DIK_RIGHT},
    {"DIK_END", DIK_END},
    {"DIK_DOWN", DIK_DOWN},
    {"DIK_NEXT", DIK_NEXT},
    {"DIK_INSERT", DIK_INSERT},
    {"DIK_DELETE", DIK_DELETE},
    {"DIK_LWIN", DIK_LWIN},
    {"DIK_RWIN", DIK_RWIN},
    {"DIK_APPS", DIK_APPS},
    {"DIK_POWER", DIK_POWER},
    {"DIK_SLEEP", DIK_SLEEP},
    {"DIK_WAKE", DIK_WAKE},
    {"DIK_WEBSEARCH", DIK_WEBSEARCH},
    {"DIK_WEBFAVORITES", DIK_WEBFAVORITES},
    {"DIK_WEBREFRESH", DIK_WEBREFRESH},
    {"DIK_WEBSTOP", DIK_WEBSTOP},
    {"DIK_WEBFORWARD", DIK_WEBFORWARD},
    {"DIK_WEBBACK", DIK_WEBBACK},
    {"DIK_MYCOMPUTER", DIK_MYCOMPUTER},
    {"DIK_MAIL", DIK_MAIL},
    {"DIK_MEDIASELECT", DIK_MEDIASELECT}
};

//==================================================
// マウス
//==================================================

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

//==================================================
// コントローラー
//==================================================

/// @brief コントローラーの状態
XINPUT_STATE sControllerState[4] = {};
/// @brief 前回のコントローラーの状態
XINPUT_STATE sPreControllerState[4] = {};
/// @brief コントローラーの状態の差分
XINPUT_STATE sControllerStateDelta[4] = {};
/// @brief 前回のコントローラーの状態の差分
XINPUT_STATE sPreControllerStateDelta[4] = {};
/// @brief コントローラーの接続状態
bool sControllerConnected[4] = { false, false, false, false };
/// @brief 前回のコントローラーの接続状態
bool sPreControllerConnected[4] = { false, false, false, false };

/// @brief コントローラーのスティックのデッドゾーン
SHORT sControllerStickDeadZone = 4096;
/// @brief コントローラーの振動状態
XINPUT_VIBRATION vibration[4] = {};

//==================================================
// 関数マップ
//==================================================

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
std::unordered_map<Input::CurrentOption, std::unordered_map<Input::DownStateOption, std::function<bool(int, int)>>> sGetXBoxButtonFunctions = {
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

//==================================================
// 値マップ
//==================================================

std::unordered_map<int, XBoxButtonCode> sXBoxButtonCodeMap = {
    { 0, XBoxButtonCode::UP },
    { 1, XBoxButtonCode::DOWN },
    { 2, XBoxButtonCode::LEFT },
    { 3, XBoxButtonCode::RIGHT },
    { 4, XBoxButtonCode::START },
    { 5, XBoxButtonCode::BACK },
    { 6, XBoxButtonCode::LEFT_THUMB },
    { 7, XBoxButtonCode::RIGHT_THUMB },
    { 8, XBoxButtonCode::LEFT_SHOULDER },
    { 9, XBoxButtonCode::RIGHT_SHOULDER },
    { 10, XBoxButtonCode::A },
    { 11, XBoxButtonCode::B },
    { 12, XBoxButtonCode::X },
    { 13, XBoxButtonCode::Y }
};

} // namespace

void Input::SetMainScreen(ScreenBuffer *screen) {
    sMainScreen = screen;
}

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
        memcpy(&sPreControllerStateDelta[i], &sControllerStateDelta[i], sizeof(XINPUT_STATE));
        memcpy(&sPreControllerConnected[i], &sControllerConnected[i], sizeof(bool));

        sControllerConnected[i] = true; // 初期状態では接続されていると仮定
        ZeroMemory(&sControllerState[i], sizeof(XINPUT_STATE));
        DWORD dw = XInputGetState(i, &sControllerState[i]);
        // コントローラーが接続されていない場合は状態をクリア
        if (dw == ERROR_DEVICE_NOT_CONNECTED) {
            ZeroMemory(&sControllerState[i], sizeof(XINPUT_STATE));
            continue;
            sControllerConnected[i] = false;
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

        // 差分を計算
        sControllerStateDelta[i].Gamepad.bLeftTrigger =
            sControllerState[i].Gamepad.bLeftTrigger - sPreControllerState[i].Gamepad.bLeftTrigger;
        sControllerStateDelta[i].Gamepad.bRightTrigger =
            sControllerState[i].Gamepad.bRightTrigger - sPreControllerState[i].Gamepad.bRightTrigger;
        sControllerStateDelta[i].Gamepad.sThumbLX =
            sControllerState[i].Gamepad.sThumbLX - sPreControllerState[i].Gamepad.sThumbLX;
        sControllerStateDelta[i].Gamepad.sThumbLY =
            sControllerState[i].Gamepad.sThumbLY - sPreControllerState[i].Gamepad.sThumbLY;
        sControllerStateDelta[i].Gamepad.sThumbRX =
            sControllerState[i].Gamepad.sThumbRX - sPreControllerState[i].Gamepad.sThumbRX;
        sControllerStateDelta[i].Gamepad.sThumbRY =
            sControllerState[i].Gamepad.sThumbRY - sPreControllerState[i].Gamepad.sThumbRY;
    }
    
    // マウスの座標を取得
    sPreMousePos = sMousePos;
    GetCursorPos(&sMousePos);
    ScreenToClient(sWinApp->GetWindowHandle(), &sMousePos);
}

InputDeviceType Input::GetCurrentInputDeviceType() {
    // 初期化済みフラグをチェック
    if (!sIsInitialized) {
        Log("Input is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    InputDeviceType currentDeviceType = InputDeviceType::None;
    
    // キーボードが押されているかチェック
    for (const auto keyCode : kDikMap) {
        if (IsKey(CurrentOption::Current, DownStateOption::Down, keyCode.second)) {
            currentDeviceType = InputDeviceType::Keyboard;
            return currentDeviceType;
        }
    }
    // マウスボタンが押されているかチェック
    for (int i = 0; i < 5; ++i) { // マウスボタンは0〜4まで
        if (IsMouseButton(CurrentOption::Current, DownStateOption::Down, i)) {
            currentDeviceType = InputDeviceType::Mouse;
            return currentDeviceType;
        }
    }
    // マウスカーソルやホイールの位置が変化しているかチェック
    if (IsMousePos(DownStateOption::Down, AxisOption::X) ||
        IsMousePos(DownStateOption::Down, AxisOption::Y) ||
        IsMousePos(DownStateOption::Down, AxisOption::Z)) {
        currentDeviceType = InputDeviceType::Mouse;
        return currentDeviceType;
    }
    // コントローラーのボタンが押されているかチェック
    for (int i = 0; i < 4; ++i) { // コントローラーは最大4つまで
        if (sControllerConnected[i]) {
            for (int j = 0; j < static_cast<int>(XBoxButtonCode::Count); ++j) {
                int buttonCode = static_cast<int>(sXBoxButtonCodeMap.at(j));
                if (IsXBoxButton(CurrentOption::Current, DownStateOption::Down, buttonCode, i)) {
                    currentDeviceType = InputDeviceType::XBoxController;
                    return currentDeviceType;
                }
            }
            // コントローラーのスティックやトリガーの状態をチェック
            if (IsXBoxTrigger(DownStateOption::Down, LeftRightOption::Left, 1, i) ||
                IsXBoxTrigger(DownStateOption::Down, LeftRightOption::Right, 1, i) ||
                IsXBoxStick(DownStateOption::Down, LeftRightOption::Left, AxisOption::X, 4096, i) ||
                IsXBoxStick(DownStateOption::Down, LeftRightOption::Left, AxisOption::Y, 4096, i) ||
                IsXBoxStick(DownStateOption::Down, LeftRightOption::Right, AxisOption::X, 4096, i) ||
                IsXBoxStick(DownStateOption::Down, LeftRightOption::Right, AxisOption::Y, 4096, i)) {
                currentDeviceType = InputDeviceType::XBoxController;
                return currentDeviceType;
            }
        }
    }
    
    return currentDeviceType;
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

bool Input::IsMousePos(DownStateOption downStateOption, AxisOption axisOption, int threshold) {
    int currentValue = 0;
    int previousValue = 0;

    // Z軸(マウスホイール)の場合は閾値を1に設定
    if (axisOption == AxisOption::Z) {
        threshold = 1;
    }
    
    // マウスの位置を取得する関数を呼び出す
    auto itCurrent = sGetMousePositionFunctions.find(CurrentOption::Current);
    if (itCurrent == sGetMousePositionFunctions.end()) {
        return false;
    }
    auto itAxis = itCurrent->second.find(axisOption);
    if (itAxis == itCurrent->second.end()) {
        return false;
    }
    auto itValue = itAxis->second.find(ValueOption::Delta);
    if (itValue == itAxis->second.end()) {
        return false;
    }
    auto getMousePositionFunction = itValue->second;
    if (getMousePositionFunction) {
        currentValue = getMousePositionFunction();
    }
    // 取得したい状態がDownの場合は現在の値が閾値以上かどうかをチェック
    if (downStateOption == DownStateOption::Down) {
        return currentValue >= threshold;
    }
    
    auto itPreCurrent = sGetMousePositionFunctions.find(CurrentOption::Previous);
    if (itPreCurrent == sGetMousePositionFunctions.end()) {
        return false;
    }
    auto itPreAxis = itPreCurrent->second.find(axisOption);
    if (itPreAxis == itPreCurrent->second.end()) {
        return false;
    }
    auto itPreValue = itPreAxis->second.find(ValueOption::Delta);
    if (itPreValue == itPreAxis->second.end()) {
        return false;
    }
    auto getPreMousePositionFunction = itPreValue->second;
    if (getPreMousePositionFunction) {
        previousValue = getPreMousePositionFunction();
    }

    // 押下状態の判定
    switch (downStateOption) {
        case DownStateOption::Trigger:
            return currentValue >= threshold && previousValue < threshold;
        case DownStateOption::Release:
            return currentValue < threshold && previousValue >= threshold;
        default:
            break;
    }
    
    return false;
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
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率と左上座標を考慮してマウス座標を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        Vector2 leftTop = sMainScreen->GetCurrentLeftTopPos();
        return static_cast<int>((sMousePos.x - leftTop.x) / scale.x);
    }
#endif
    return static_cast<int>(sMousePos.x);
}

int Input::GetMouseY() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率と左上座標を考慮してマウス座標を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        Vector2 leftTop = sMainScreen->GetCurrentLeftTopPos();
        return static_cast<int>((sMousePos.y - leftTop.y) / scale.y);
    }
#endif
    return static_cast<int>(sMousePos.y);
}

int Input::GetMouseDeltaX() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率を考慮してマウス座標の差分を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        return static_cast<int>(sMouseState.lX * scale.x);
    }
#endif
    return static_cast<int>(sMouseState.lX);
}

int Input::GetMouseDeltaY() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率を考慮してマウス座標の差分を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        return static_cast<int>(sMouseState.lY * scale.y);
    }
#endif
    return static_cast<int>(sMouseState.lY);
}

int Input::GetPreMouseX() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率と左上座標を考慮してマウス座標を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        Vector2 leftTop = sMainScreen->GetCurrentLeftTopPos();
        return static_cast<int>((sPreMousePos.x - leftTop.x) * scale.x);
    }
#endif
    return static_cast<int>(sPreMousePos.x);
}

int Input::GetPreMouseY() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率と左上座標を考慮してマウス座標を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        Vector2 leftTop = sMainScreen->GetCurrentLeftTopPos();
        return static_cast<int>((sPreMousePos.y - leftTop.y) * scale.y);
    }
#endif
    return static_cast<int>(sPreMousePos.y);
}

int Input::GetPreMouseDeltaX() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率を考慮してマウス座標の差分を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        return static_cast<int>(sPreMouseState.lX * scale.x);
    }
#endif
    return static_cast<int>(sPreMouseState.lX);
}

int Input::GetPreMouseDeltaY() {
#ifdef _DEBUG
    if (sMainScreen) {
        // スクリーンの拡大率を考慮してマウス座標の差分を補正
        Vector2 scale = sMainScreen->GetCurrentScale();
        return static_cast<int>(sPreMouseState.lY * scale.y);
    }
#endif
    return static_cast<int>(sPreMouseState.lY);
}

int Input::GetMouseWheel() {
    return static_cast<int>(sMouseState.lZ);
}

int Input::GetPreMouseWheel() {
    return static_cast<int>(sPreMouseState.lZ);
}

bool Input::IsXBoxTrigger(DownStateOption downStateOption, LeftRightOption leftRightOption, int threshold, int index) {
    int currentValue = 0;
    int previousValue = 0;

    // コントローラーのトリガーの押下状態を取得する関数を呼び出す
    auto itCurrent = sGetXBoxTriggerFunctions.find(CurrentOption::Current);
    if (itCurrent == sGetXBoxTriggerFunctions.end()) {
        return false;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return false;
    }
    auto itValue = itLeftRight->second.find(ValueOption::Actual);
    if (itValue == itLeftRight->second.end()) {
        return false;
    }
    auto getXBoxTriggerFunction = itValue->second;
    if (getXBoxTriggerFunction) {
        currentValue = getXBoxTriggerFunction(index);
    }
    // 取得したい状態がDownの場合は現在の値が閾値以上かどうかをチェック
    if (downStateOption == DownStateOption::Down) {
        return currentValue >= threshold;
    }

    auto itPreCurrent = sGetXBoxTriggerFunctions.find(CurrentOption::Previous);
    if (itPreCurrent == sGetXBoxTriggerFunctions.end()) {
        return false;
    }
    auto itPreLeftRight = itPreCurrent->second.find(leftRightOption);
    if (itPreLeftRight == itPreCurrent->second.end()) {
        return false;
    }
    auto itPreValue = itPreLeftRight->second.find(ValueOption::Actual);
    if (itPreValue == itPreLeftRight->second.end()) {
        return false;
    }
    auto getPreXBoxTriggerFunction = itPreValue->second;
    if (getPreXBoxTriggerFunction) {
        previousValue = getPreXBoxTriggerFunction(index);
    }

    // 押下状態の判定
    switch (downStateOption) {
        case DownStateOption::Trigger:
            return currentValue >= threshold && previousValue < threshold;
        case DownStateOption::Release:
            return currentValue < threshold && previousValue >= threshold;
        default:
            break;
    }
    
    return false;
}

int Input::GetXBoxTrigger(CurrentOption currentOption, LeftRightOption leftRightOption, ValueOption valueOption, int index) {
    // コントローラーのトリガーを取得する関数を呼び出す
    auto itCurrent = sGetXBoxTriggerFunctions.find(currentOption);
    if (itCurrent == sGetXBoxTriggerFunctions.end()) {
        return 0;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return 0;
    }
    auto itValue = itLeftRight->second.find(valueOption);
    if (itValue == itLeftRight->second.end()) {
        return 0;
    }
    auto getXBoxTriggerFunction = itValue->second;
    if (getXBoxTriggerFunction) {
        return getXBoxTriggerFunction(index);
    }
    
    return 0;
}

float Input::GetXBoxTriggerRatio(CurrentOption currentOption, LeftRightOption leftRightOption, ValueOption valueOption, int index) {
    // コントローラーのトリガー比率を取得する関数を呼び出す
    auto itCurrent = sGetXBoxTriggerRatioFunctions.find(currentOption);
    if (itCurrent == sGetXBoxTriggerRatioFunctions.end()) {
        return 0.0f;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return 0.0f;
    }
    auto itValue = itLeftRight->second.find(valueOption);
    if (itValue == itLeftRight->second.end()) {
        return 0.0f;
    }
    auto getXBoxTriggerRatioFunction = itValue->second;
    if (getXBoxTriggerRatioFunction) {
        return getXBoxTriggerRatioFunction(index);
    }
    
    return 0.0f;
}

int Input::GetXBoxLeftTrigger(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.bLeftTrigger);
}

int Input::GetXBoxRightTrigger(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.bRightTrigger);
}

int Input::GetXBoxLeftTriggerDelta(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.bLeftTrigger);
}

int Input::GetXBoxRightTriggerDelta(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.bRightTrigger);
}

float Input::GetXBoxLeftTriggerRatio(int index) {
    // トリガーの値を0から1の範囲に変換
    return static_cast<float>(GetXBoxLeftTrigger(index)) / 255.0f;
}

float Input::GetXBoxRightTriggerRatio(int index) {
    // トリガーの値を0から1の範囲に変換
    return static_cast<float>(GetXBoxRightTrigger(index)) / 255.0f;
}

float Input::GetXBoxLeftTriggerDeltaRatio(int index) {
    // トリガーの差分値を0から1の範囲に変換
    return static_cast<float>(GetXBoxLeftTriggerDelta(index)) / 255.0f;
}

float Input::GetXBoxRightTriggerDeltaRatio(int index) {
    // トリガーの差分値を0から1の範囲に変換
    return static_cast<float>(GetXBoxRightTriggerDelta(index)) / 255.0f;
}

bool Input::IsXBoxStick(DownStateOption downStateOption, LeftRightOption leftRightOption, AxisOption axisOption, int threshold, int index) {
    int currentValue = 0;
    int previousValue = 0;
    
    // コントローラーのスティックの値を取得する関数を呼び出す
    auto itCurrent = sGetXBoxStickFunctions.find(CurrentOption::Current);
    if (itCurrent == sGetXBoxStickFunctions.end()) {
        return false;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return false;
    }
    auto itAxis = itLeftRight->second.find(axisOption);
    if (itAxis == itLeftRight->second.end()) {
        return false;
    }
    auto getXBoxStickFunction = itAxis->second.find(ValueOption::Actual);
    if (getXBoxStickFunction == itAxis->second.end()) {
        return false;
    }
    auto getXBoxStickValueFunction = getXBoxStickFunction->second;
    if (getXBoxStickValueFunction) {
        currentValue = getXBoxStickValueFunction(index);
    }
    // 取得したい状態がDownの場合は現在の値が閾値以上かどうかをチェック
    if (downStateOption == DownStateOption::Down) {
        return currentValue >= threshold;
    }
    
    auto itPreCurrent = sGetXBoxStickFunctions.find(CurrentOption::Previous);
    if (itPreCurrent == sGetXBoxStickFunctions.end()) {
        return false;
    }
    auto itPreLeftRight = itPreCurrent->second.find(leftRightOption);
    if (itPreLeftRight == itPreCurrent->second.end()) {
        return false;
    }
    auto itPreAxis = itPreLeftRight->second.find(axisOption);
    if (itPreAxis == itPreLeftRight->second.end()) {
        return false;
    }
    auto getPreXBoxStickFunction = itPreAxis->second.find(ValueOption::Actual);
    if (getPreXBoxStickFunction == itPreAxis->second.end()) {
        return false;
    }
    auto getPreXBoxStickValueFunction = getPreXBoxStickFunction->second;
    if (getPreXBoxStickValueFunction) {
        previousValue = getPreXBoxStickValueFunction(index);
    }
    
    // 押下状態の判定
    switch (downStateOption) {
        case DownStateOption::Trigger:
            return currentValue >= threshold && previousValue < threshold;
        case DownStateOption::Release:
            return currentValue < threshold && previousValue >= threshold;
        default:
            break;
    }

    return false;
}

int Input::GetXBoxStick(CurrentOption currentOption, LeftRightOption leftRightOption, AxisOption axisOption, ValueOption valueOption, int index) {
    // コントローラーのスティックを取得する関数を呼び出す
    auto itCurrent = sGetXBoxStickFunctions.find(currentOption);
    if (itCurrent == sGetXBoxStickFunctions.end()) {
        return 0;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return 0;
    }
    auto itAxis = itLeftRight->second.find(axisOption);
    if (itAxis == itLeftRight->second.end()) {
        return 0;
    }
    auto itValue = itAxis->second.find(valueOption);
    if (itValue == itAxis->second.end()) {
        return 0;
    }
    auto getXBoxStickFunction = itValue->second;
    if (getXBoxStickFunction) {
        return getXBoxStickFunction(index);
    }
    
    return 0;
}

int Input::GetPreXBoxLeftTrigger(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.bLeftTrigger);
}

int Input::GetPreXBoxRightTrigger(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.bRightTrigger);
}

int Input::GetPreXBoxLeftTriggerDelta(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.bLeftTrigger);
}

int Input::GetPreXBoxRightTriggerDelta(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.bRightTrigger);
}

float Input::GetPreXBoxLeftTriggerRatio(int index) {
    return static_cast<float>(GetPreXBoxLeftTrigger(index)) / 255.0f;
}

float Input::GetPreXBoxRightTriggerRatio(int index) {
    return static_cast<float>(GetPreXBoxRightTrigger(index)) / 255.0f;
}

float Input::GetPreXBoxLeftTriggerDeltaRatio(int index) {
    return static_cast<float>(GetPreXBoxLeftTriggerDelta(index)) / 255.0f;
}

float Input::GetPreXBoxRightTriggerDeltaRatio(int index) {
    return static_cast<float>(GetPreXBoxRightTriggerDelta(index)) / 255.0f;
}

float Input::GetXBoxStickRatio(CurrentOption currentOption, LeftRightOption leftRightOption, AxisOption axisOption, ValueOption valueOption, int index) {
    // コントローラーのスティック比率を取得する関数を呼び出す
    auto itCurrent = sGetXBoxStickRatioFunctions.find(currentOption);
    if (itCurrent == sGetXBoxStickRatioFunctions.end()) {
        return 0.0f;
    }
    auto itLeftRight = itCurrent->second.find(leftRightOption);
    if (itLeftRight == itCurrent->second.end()) {
        return 0.0f;
    }
    auto itAxis = itLeftRight->second.find(axisOption);
    if (itAxis == itLeftRight->second.end()) {
        return 0.0f;
    }
    auto itValue = itAxis->second.find(valueOption);
    if (itValue == itAxis->second.end()) {
        return 0.0f;
    }
    auto getXBoxStickRatioFunction = itValue->second;
    if (getXBoxStickRatioFunction) {
        return getXBoxStickRatioFunction(index);
    }
    
    return 0.0f;
}

int Input::GetXBoxLeftStickX(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbLX);
}

int Input::GetXBoxLeftStickY(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbLY);
}

int Input::GetXBoxLeftStickDeltaX(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.sThumbLX);
}

int Input::GetXBoxLeftStickDeltaY(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.sThumbLY);
}

float Input::GetXBoxLeftStickRatioX(int index) {
    return std::clamp(static_cast<float>(GetXBoxLeftStickX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxLeftStickRatioY(int index) {
    return std::clamp(static_cast<float>(GetXBoxLeftStickY(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxLeftStickDeltaRatioX(int index) {
    return std::clamp(static_cast<float>(GetXBoxLeftStickDeltaX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxLeftStickDeltaRatioY(int index) {
    return std::clamp(static_cast<float>(GetXBoxLeftStickDeltaY(index)) / 32767.0f, -1.0f, 1.0f);
}

int Input::GetXBoxRightStickX(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbRX);
}

int Input::GetXBoxRightStickY(int index) {
    return static_cast<int>(sControllerState[index].Gamepad.sThumbRY);
}

int Input::GetXBoxRightStickDeltaX(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.sThumbRX);
}

int Input::GetXBoxRightStickDeltaY(int index) {
    return static_cast<int>(sControllerStateDelta[index].Gamepad.sThumbRY);
}

float Input::GetXBoxRightStickRatioX(int index) {
    return std::clamp(static_cast<float>(GetXBoxRightStickX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxRightStickRatioY(int index) {
    return std::clamp(static_cast<float>(GetXBoxRightStickY(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxRightStickDeltaRatioX(int index) {
    return std::clamp(static_cast<float>(GetXBoxRightStickDeltaX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetXBoxRightStickDeltaRatioY(int index) {
    return std::clamp(static_cast<float>(GetXBoxRightStickDeltaY(index)) / 32767.0f, -1.0f, 1.0f);
}

int Input::GetPreXBoxLeftStickX(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.sThumbLX);
}

int Input::GetPreXBoxLeftStickY(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.sThumbLY);
}

int Input::GetPreXBoxLeftStickDeltaX(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.sThumbLX);
}

int Input::GetPreXBoxLeftStickDeltaY(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.sThumbLY);
}

float Input::GetPreXBoxLeftStickRatioX(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxLeftStickX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxLeftStickRatioY(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxLeftStickY(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxLeftStickDeltaRatioX(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxLeftStickDeltaX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxLeftStickDeltaRatioY(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxLeftStickDeltaY(index)) / 32767.0f, -1.0f, 1.0f);
}

int Input::GetPreXBoxRightStickX(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.sThumbRX);
}

int Input::GetPreXBoxRightStickY(int index) {
    return static_cast<int>(sPreControllerState[index].Gamepad.sThumbRY);
}

int Input::GetPreXBoxRightStickDeltaX(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.sThumbRX);
}

int Input::GetPreXBoxRightStickDeltaY(int index) {
    return static_cast<int>(sPreControllerStateDelta[index].Gamepad.sThumbRY);
}

float Input::GetPreXBoxRightStickRatioX(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxRightStickX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxRightStickRatioY(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxRightStickY(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxRightStickDeltaRatioX(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxRightStickDeltaX(index)) / 32767.0f, -1.0f, 1.0f);
}

float Input::GetPreXBoxRightStickDeltaRatioY(int index) {
    return std::clamp(static_cast<float>(GetPreXBoxRightStickDeltaY(index)) / 32767.0f, -1.0f, 1.0f);
}

bool Input::IsXBoxButton(CurrentOption currentOption, DownStateOption downStateOption, int button, int index) {
    // コントローラーのボタンの押下状態を取得する関数を呼び出す
    auto itCurrent = sGetXBoxButtonFunctions.find(currentOption);
    if (itCurrent == sGetXBoxButtonFunctions.end()) {
        return false;
    }
    auto itDownState = itCurrent->second.find(downStateOption);
    if (itDownState == itCurrent->second.end()) {
        return false;
    }
    auto getXBoxButtonFunction = itDownState->second;
    if (getXBoxButtonFunction) {
        return getXBoxButtonFunction(button, index);
    }
    
    return false;
}

bool Input::IsXBoxButtonDown(int button, int index) {
    return sControllerState[index].Gamepad.wButtons & static_cast<WORD>(button) ? true : false;
}

bool Input::IsPreXBoxButtonDown(int button, int index) {
    return sPreControllerState[index].Gamepad.wButtons & static_cast<WORD>(button) ? true : false;
}

bool Input::IsXBoxButtonTrigger(int button, int index) {
    return (sControllerState[index].Gamepad.wButtons & static_cast<WORD>(button)) != 0 &&
        (sPreControllerState[index].Gamepad.wButtons & static_cast<WORD>(button)) == 0;
}

bool Input::IsXBoxButtonRelease(int button, int index) {
    return (sControllerState[index].Gamepad.wButtons & static_cast<WORD>(button)) == 0 &&
        (sPreControllerState[index].Gamepad.wButtons & static_cast<WORD>(button)) != 0;
}

int Input::GetXBoxConnectedCount() {
    // XInputで接続されているコントローラーの数を取得
    DWORD dwResult = 0;
    for (int i = 0; i < 4; ++i) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        dwResult = XInputGetState(i, &state);
        if (dwResult == ERROR_SUCCESS) {
            return i + 1; // 接続されているコントローラーの数を返す
        }
    }
    return 0; // 接続されていない場合は0を返す
}

bool Input::IsXBoxConnected(int index) {
    return (index >= 0 && index < 4) ? sControllerConnected[index] : false;
}

bool Input::IsPreXBoxConnected(int index) {
    return (index >= 0 && index < 4) ? sPreControllerConnected[index] : false;
}

void Input::SetXBoxVibration(int index, int leftMotor, int rightMotor) {
    // 値が-1でなければ振動を設定
    if (leftMotor > -1) {
        vibration[index].wLeftMotorSpeed = static_cast<WORD>(leftMotor);
    }
    if (rightMotor > -1) {
        vibration[index].wRightMotorSpeed = static_cast<WORD>(rightMotor);
    }
    // コントローラーの振動を設定
    XInputSetState(index, &vibration[index]);
}

void Input::StopXBoxVibration(int index) {
    vibration[index].wLeftMotorSpeed = 0;
    vibration[index].wRightMotorSpeed = 0;
    XInputSetState(index, &vibration[index]);
}

int Input::GetXBoxVibration(int index, LeftRightOption leftRightOption) {
    switch (leftRightOption) {
        case Input::LeftRightOption::Left:
            return vibration[index].wLeftMotorSpeed;
            break;
        case Input::LeftRightOption::Right:
            return vibration[index].wRightMotorSpeed;
            break;
    }
    return 0;
}

} // namespace KashipanEngine