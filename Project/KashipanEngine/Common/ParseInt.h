#pragma once
#include <string>
#include <stdexcept>

namespace KashipanEngine {

/// @brief 文字列から整数を解析する関数
/// @param str 解析する文字列 (例: "0b1010", "0x1A", "42")
/// @return 解析された整数値
int ParseInt(const std::string &str) {
    size_t pos = 0;
    int base = 10;
    // 先頭の空白をスキップ
    while (pos < str.size() && isspace(str[pos])) {
        pos++;
    }
    // 0x または 0b プレフィックスをチェック
    if (str.compare(pos, 2, "0x") == 0 || str.compare(pos, 2, "0X") == 0) {
        base = 16;
        pos += 2;
    } else if (str.compare(pos, 2, "0b") == 0 || str.compare(pos, 2, "0B") == 0) {
        base = 2;
        pos += 2;
    } else if (str[pos] == '0' && pos + 1 < str.size() && isdigit(str[pos + 1])) {
        base = 8; // オクタル
        pos++;
    }
    // 数字部分を解析
    int value = std::stoi(str.substr(pos), nullptr, base);
    return value;
}

} // namespace KashipanEngine