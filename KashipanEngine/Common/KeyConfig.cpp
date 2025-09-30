#include "Common/JsoncLoader.h"
#include "Common/KeyConfig/DefineMaps.h"
#include "KeyConfig.h"

namespace KashipanEngine {
using namespace KeyConfigDefineMaps;

const KeyConfig::ConfigData &KeyConfig::operator[](const std::string &actionName) const {
    auto it = keyConfigMap_.find(actionName);
    if (it != keyConfigMap_.end()) {
        return it->second; // キーが見つかった場合はその値を返す
    }
    throw std::out_of_range("Action name not found in key config map.");
}

void KeyConfig::LoadFromJson(const std::string &filePath) {
    keyConfigMap_.clear();
    Json jsonData = LoadJsonc(filePath);

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        ConfigData configData;
        configData.actionName = it.key();

        const Json &config = it.value();
        std::string inputType = config["InputType"].get<std::string>();

        if (inputType == "Digital") {
            configData.returnType = "bool";
        } else if (inputType == "Analog") {
            configData.returnType = "float";
        } else {
            throw std::invalid_argument("Unknown input type: " + inputType);
        }

        for (const auto &keyBinding : config["Bindings"]) {
            KeyBinding keyBindingData;
            std::string device = keyBinding["Device"].get<std::string>();

            if (device == "Keyboard") {
                keyBindingData = GetKeyboardKeyBinding(keyBinding);
            } else if (device == "Mouse") {
                keyBindingData = GetMouseKeyBinding(keyBinding);
            } else if (device == "Controller") {
                keyBindingData = GetControllerKeyBinding(keyBinding);
            } else {
                throw std::invalid_argument("Unknown device type: " + device);
            }

            configData.keyBindings.push_back(keyBindingData);
        }

        keyConfigMap_[configData.actionName] = configData;
    }
}

KeyConfig::ActionValue KeyConfig::GetInputValue(const std::string &actionName) {
    auto it = keyConfigMap_.find(actionName);
    if (it != keyConfigMap_.end()) {
        const ConfigData &configData = it->second;
        ActionValue actionValue;
        if (configData.returnType == "float") {
            actionValue = 0.0f;
        } else if (configData.returnType == "bool") {
            actionValue = false;
        }

        for (const auto &binding : configData.keyBindings) {
            if (configData.returnType == "float") {
                float value = Input::Get<float>(
                    binding.deviceType,
                    binding.actionType,
                    binding.keyCode,
                    Input::InputTypeOption::Analog,
                    binding.axisType,
                    binding.leftRightOption,
                    binding.threshold
                );
                actionValue = std::get<float>(actionValue) + (value * binding.scale);

            } else if (configData.returnType == "bool") {
                bool isPressed = Input::Get<bool>(
                    binding.deviceType,
                    binding.actionType,
                    binding.keyCode,
                    Input::InputTypeOption::Digital,
                    binding.axisType,
                    binding.leftRightOption,
                    binding.threshold * static_cast<int>(binding.scale)
                );
                if (isPressed) {
                    // いずれかのキーが押されている場合は true を返す
                    actionValue = true;
                }
            }
        }
        return actionValue;
    }

    throw std::out_of_range("Action name not found in key config map.");
}

const KeyConfig::ConfigData &KeyConfig::GetKeyConfig(const std::string &actionName) const {
    auto it = keyConfigMap_.find(actionName);
    if (it != keyConfigMap_.end()) {
        return it->second;
    }
    throw std::out_of_range("Action name not found in key config map.");
}

KeyConfig::KeyBinding KeyConfig::GetKeyboardKeyBinding(const Json &jsonData) const {
    KeyBinding keyBindingData;
    keyBindingData.deviceType = InputDeviceType::Keyboard;
    
    std::string input = jsonData["Input"].get<std::string>();
    auto itMap = kKeyboardKeyMap.find(input);
    if (itMap != kKeyboardKeyMap.end()) {
        keyBindingData.keyCode = itMap->second;
    } else {
        throw std::invalid_argument("Unknown keyboard input: " + input);
    }

    std::string event = jsonData["Event"].get<std::string>();
    keyBindingData.actionType = GetActionType(event);
    keyBindingData.scale = GetScale(jsonData);
    
    return keyBindingData;
}

KeyConfig::KeyBinding KeyConfig::GetMouseKeyBinding(const Json &jsonData) const {
    KeyBinding keyBindingData;
    keyBindingData.deviceType = InputDeviceType::Mouse;
    
    std::string input = jsonData["Input"].get<std::string>();
    // 入力が "Cursour"、"Wheel" の場合は別の処理
    if (input == "Cursor") {
        keyBindingData.keyCode = -1;
        std::string axis = jsonData["Axis"].get<std::string>();
        if (axis == "X") {
            keyBindingData.axisType = Input::AxisOption::X;
        } else if (axis == "Y") {
            keyBindingData.axisType = Input::AxisOption::Y;
        } else {
            throw std::invalid_argument("Unknown mouse axis: " + axis);
        }

    } else if (input == "Wheel") {
        keyBindingData.keyCode = -1;
        keyBindingData.axisType = Input::AxisOption::Z;

    } else {
        auto itMap = kMouseButtonIndexMap.find(input);
        if (itMap != kMouseButtonIndexMap.end()) {
            keyBindingData.keyCode = itMap->second;
        } else {
            throw std::invalid_argument("Unknown mouse input: " + input);
        }
    }

    if (jsonData.contains("DeadZone")) {
        keyBindingData.threshold = jsonData["DeadZone"].get<int>();
    } else {
        keyBindingData.threshold = 64;
    }

    std::string event = jsonData["Event"].get<std::string>();
    keyBindingData.actionType = GetActionType(event);
    keyBindingData.scale = GetScale(jsonData);
    
    return keyBindingData;
}

KeyConfig::KeyBinding KeyConfig::GetControllerKeyBinding(const Json &jsonData) const {
    KeyBinding keyBindingData;
    keyBindingData.deviceType = InputDeviceType::XBoxController;
    
    std::string input = jsonData["Input"].get<std::string>();
    // 入力が "LeftStick"、"RightStick"、"LeftTrigger"、"RightTrigger" の場合は別の処理
    if (input == "LeftStick") {
        keyBindingData.keyCode = -1;
        keyBindingData.leftRightOption = Input::LeftRightOption::Left;
        keyBindingData.axisType = GetAxisType(jsonData["Axis"].get<std::string>());

    } else if (input == "RightStick") {
        keyBindingData.keyCode = -1;
        keyBindingData.leftRightOption = Input::LeftRightOption::Right;
        keyBindingData.axisType = GetAxisType(jsonData["Axis"].get<std::string>());
    
    } else if (input == "LeftTrigger") {
        keyBindingData.keyCode = -1;
        keyBindingData.leftRightOption = Input::LeftRightOption::Left;
        keyBindingData.axisType = GetAxisType(jsonData["Axis"].get<std::string>());
    
    } else if (input == "RightTrigger") {
        keyBindingData.keyCode = -1;
        keyBindingData.leftRightOption = Input::LeftRightOption::Right;
        keyBindingData.axisType = GetAxisType(jsonData["Axis"].get<std::string>());
    
    } else {
        auto itMap = kXInputButtonMap.find(input);
        if (itMap != kXInputButtonMap.end()) {
            keyBindingData.keyCode = itMap->second;
        } else {
            throw std::invalid_argument("Unknown controller input: " + input);
        }
    }

    if (jsonData.contains("DeadZone")) {
        keyBindingData.threshold = jsonData["DeadZone"].get<int>();
    } else {
        if (input == "LeftStick" || input == "RightStick") {
            keyBindingData.threshold = 4096;
        } else {
            keyBindingData.threshold = 64;
        }
    }

    std::string event = jsonData["Event"].get<std::string>();
    keyBindingData.actionType = GetActionType(event);
    keyBindingData.scale = GetScale(jsonData);

    return keyBindingData;
}

Input::DownStateOption KeyConfig::GetActionType(const std::string &actionTypeStr) const {
    // 文字列が空の場合はデフォルトの状態を返す
    if (actionTypeStr.empty()) {
        return Input::DownStateOption::Down;
    }

    if (actionTypeStr == "Down") {
        return Input::DownStateOption::Down;
    } else if (actionTypeStr == "Trigger") {
        return Input::DownStateOption::Trigger;
    } else if (actionTypeStr == "Release") {
        return Input::DownStateOption::Release;
    } else {
        throw std::invalid_argument("Unknown action type: " + actionTypeStr);
    }
}

Input::AxisOption KeyConfig::GetAxisType(const std::string &axisTypeStr) const {
    if (axisTypeStr == "X") {
        return Input::AxisOption::X;
    } else if (axisTypeStr == "Y") {
        return Input::AxisOption::Y;
    } else if (axisTypeStr == "Z") {
        return Input::AxisOption::Z;
    } else {
        throw std::invalid_argument("Unknown axis type: " + axisTypeStr);
    }
}

float KeyConfig::GetScale(const Json &jsonData) const {
    if (jsonData.contains("Scale")) {
        float scale = jsonData["Scale"].get<float>();
        // スケールが 1.0f か -1.0f でない場合は符号だけを保持して 1.0f にする
        if (scale != 1.0f && scale != -1.0f) {
            scale = (scale > 0.0f) ? 1.0f : -1.0f;
        }
        return scale;
    }
    return 1.0f;
}

} // namespace KashipanEngine