#pragma once
#include <json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace KashipanEngine {

using Json = nlohmann::json;

// 基本的な読み込み・保存機能
Json LoadJsonc(const std::string &filepath);
bool SaveJsonc(const Json &jsonData, const std::string &filepath, int indent = 4);

// 安全な値取得機能
template<typename T>
std::optional<T> GetJsonValue(const Json &json, const std::string &key);

template<typename T>
T GetJsonValueOrDefault(const Json &json, const std::string &key, const T &defaultValue);

// ネストしたキーへのアクセス（例: "object.array[0].value"）
std::optional<Json> GetNestedValue(const Json &json, const std::string &path);

// JSON検証・ユーティリティ機能
bool ValidateJsonStructure(const Json &json, const std::vector<std::string> &requiredKeys);
bool IsJsonFileValid(const std::string &filepath);

// JSON合成機能
Json MergeJson(const Json &base, const Json &overlay, bool deepMerge = true);

// 配列操作
bool AppendToJsonArray(Json &json, const std::string &arrayKey, const Json &value);
bool RemoveFromJsonArray(Json &json, const std::string &arrayKey, size_t index);

// デバッグ・表示機能
std::string JsonToFormattedString(const Json &json, int indent = 4);
void PrintJson(const Json &json, const std::string &title = "JSON Data");

} // namespace KashipanEngine