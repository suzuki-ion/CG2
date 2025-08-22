#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <variant>
#include <json.hpp>
#include "Base/Input.h"

namespace KashipanEngine {
using Json = nlohmann::json;

class KeyConfig {
public:
    using ActionValue = std::variant<float, bool>;

    struct KeyBinding {
        InputDeviceType deviceType;
        int keyCode;
        // 使用する軸の種類
        Input::AxisOption axisType;
        // 操作の種類
        Input::DownStateOption actionType;
        // 左右どちらを使用するか
        Input::LeftRightOption leftRightOption;
        int threshold = 0;
        float scale = 1.0f;
    };

    struct ConfigData {
        std::string actionName;
        std::string returnType;
        std::vector<KeyBinding> keyBindings;
    };

    KeyConfig() = default;
    ~KeyConfig() = default;

    const ConfigData &operator[](const std::string &actionName) const;

    /// @brief Jsonファイルからキーコンフィグを読み込む
    /// @param filePath 読み込むJsonファイルのパス
    void LoadFromJson(const std::string &filePath);

    /// @brief 入力の値を取得
    /// @param actionName アクション名
    /// @return アクションの値
    ActionValue GetInputValue(const std::string &actionName);
    
    /// @brief キーコンフィグを追加
    /// @param actionName アクション名
    /// @param config キーコンフィグのデータ
    void AddKeyConfig(const std::string& actionName, const ConfigData & config) {
        keyConfigMap_[actionName] = config;
    }

    /// @brief キーコンフィグの取得
    /// @param actionName アクション名
    /// @return キーコンフィグの参照
    const ConfigData &GetKeyConfig(const std::string &actionName) const;

private:
    /// @brief キーボードのキーバインドデータを取得
    /// @param jsonData Jsonデータ
    /// @return キーバインドのリスト
    KeyBinding GetKeyboardKeyBinding(const Json &jsonData) const;
    /// @brief マウスのキーバインドデータを取得
    /// @param jsonData Jsonデータ
    /// @return キーバインドのリスト
    KeyBinding GetMouseKeyBinding(const Json &jsonData) const;
    /// @brief コントローラーのキーバインドデータを取得
    /// @param jsonData Jsonデータ
    /// @return キーバインドのリスト
    KeyBinding GetControllerKeyBinding(const Json &jsonData) const;

    /// @brief 操作の種類を取得
    /// @param actionTypeStr 操作の種類を表す文字列
    /// @return 操作の種類
    Input::DownStateOption GetActionType(const std::string &actionTypeStr) const;
    /// @brief 軸の種類を取得
    /// @param axisTypeStr 軸の種類を表す文字列
    /// @return 軸の種類
    Input::AxisOption GetAxisType(const std::string &axisTypeStr) const;
    /// @brief スケールを取得
    /// @param jsonData Jsonデータ
    /// @return スケールの値
    float GetScale(const Json &jsonData) const;

    // キーコンフィグのマップ
    std::unordered_map<std::string, ConfigData> keyConfigMap_;
};

} // namespace KashipanEngine