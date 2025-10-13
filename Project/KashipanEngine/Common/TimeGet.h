#pragma once
#include <chrono>
#include <string>

namespace KashipanEngine {

/// @brief PCの設定時間取得
/// @return 現在のPCの設定時間
std::chrono::zoned_seconds TimeGetZone();

/// @brief PCの設定時間を文字列で取得
/// @param format 取得する時間のフォーマット
/// @return フォーマットに従ったPCの設定時間
std::string TimeGetString(const std::string &format);

/// @brief PCの設定時間を文字列で取得
/// @param format 取得する時間のフォーマット
/// @return フォーマットに従ったPCの設定時間
std::wstring TimeGetStringW(const std::wstring &format);

} // namespace KashipanEngine