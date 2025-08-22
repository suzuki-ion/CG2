#pragma once
#include <json.hpp>
#include <any>
#include <string>
#include <AnyUnorderedMap.h>

namespace KashipanEngine {
using Json = nlohmann::json;

/// @brief 変数を動的に扱うクラス
class DynamicVariables {
public:
    struct Variable {
        std::string name;
        std::any value;
    };
    
    DynamicVariables() = default;
    ~DynamicVariables() = default;

    /// @brief Jsonファイルから変数を読み込む
    /// @param jsonFilePath 読み込むJsonファイルのパス
    void LoadVariables(const std::string &jsonFilePath);
    
    /// @brief 変数の設定
    /// @param name 変数名
    /// @param value 変数の値
    void SetVariable(const std::string &name, const Json &value);
    
    /// @brief 変数の取得
    /// @tparam T 変数の型
    /// @param name 変数名
    /// @return 変数の値
    template <typename T>
    T GetVariable(const std::string &name);

    /// @brief 変数が存在するかの確認
    /// @param name 変数名
    /// @return 変数が存在する場合はtrue、存在しない場合はfalse
    bool IsVariableExists(const std::string &name) const;

private:
    /// @brief Jsonから変数を読み込む
    /// @param json 読み込むJsonオブジェクト
    void LoadFromJson(const Json &json);

    /// @brief 変数マップ
    MyStd::AnyUnorderedMap variables_;

    /// @brief 変数の型のマップ
    const std::unordered_map<std::string, std::any> kVariableTypes_ = {
        { "int", 0 },
        { "float", 0.0f },
        { "string", std::string() },
        { "bool", false },
        { "char", '\0' },
    };
};

} // namespace KashipanEngine