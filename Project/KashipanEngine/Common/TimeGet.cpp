#include "TimeGet.h"
#include <format>

namespace KashipanEngine {

std::chrono::zoned_seconds TimeGetZone() {
    // 現在時刻を取得（UTC時刻）
    auto now = std::chrono::system_clock::now();
    // ログファイル用なのでコンマ秒は削る
    auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    // 日本時間（PCの設定時間）に変換して返す
    return std::chrono::zoned_time{ std::chrono::current_zone(), nowSeconds };
}

std::string TimeGetString(const std::string &format) {
    // フォーマットを使って年月日_時分秒の文字列に変換
    auto time = TimeGetZone();
    auto timeString = std::vformat(format, std::make_format_args(time));
    return timeString;
}

std::wstring TimeGetStringW(const std::wstring &format) {
    // フォーマットを使って年月日_時分秒の文字列に変換
    auto time = TimeGetZone();
    auto timeString = std::vformat(format, std::make_wformat_args(time));
    return timeString;
}

} // namespace KashipanEngine