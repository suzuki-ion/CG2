#include <windows.h>
#include <string>
#include "ConvertString.h"

namespace KashipanEngine {

std::wstring ConvertString(const std::string &str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring &str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

std::wstring ShiftJISToUTF16(const std::string &sjis) {
    int utf16Size = MultiByteToWideChar(932, 0, sjis.c_str(), -1, nullptr, 0);
    std::wstring utf16(utf16Size, L'\0');
    MultiByteToWideChar(932, 0, sjis.c_str(), -1, &utf16[0], utf16Size);
    return utf16;
}

std::string UTF16ToUTF8(const std::wstring &utf16) {
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(utf8Size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], utf8Size, nullptr, nullptr);
    return utf8;
}

std::string ShiftJISToUTF8(const std::string &sjis) {
    std::wstring utf16 = ShiftJISToUTF16(sjis);
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(utf8Size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], utf8Size, nullptr, nullptr);
    return utf8;
}

} // namespace KashipanEngine