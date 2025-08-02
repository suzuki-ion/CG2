#pragma once
#include <string>

namespace KashipanEngine {

/// @brief 文字列変換
/// @param str 変換元文字列
/// @return 変換後文字列
std::wstring ConvertString(const std::string &str);

/// @brief 文字列変換
/// @param str 変換元文字列
/// @return 変換後文字列
std::string ConvertString(const std::wstring &str);

} // namespace KashipanEngine

