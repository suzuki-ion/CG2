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

/// @brief ShiftJISからUTF-16へ変換
/// @param sjis 変換元ShiftJIS文字列
/// @return 変換後UTF-16文字列
std::wstring ShiftJISToUTF16(const std::string &sjis);

/// @brief UTF-16からShiftJISへ変換
/// @param utf16 変換元UTF-16文字列
/// @return 変換後ShiftJIS文字列
std::string UTF16ToUTF8(const std::wstring &utf16);

/// @brief ShiftJISからUTF-8へ変換
/// @param sjis 変換元ShiftJIS文字列
/// @return 変換後UTF-8文字列
std::string ShiftJISToUTF8(const std::string &sjis);

} // namespace KashipanEngine

