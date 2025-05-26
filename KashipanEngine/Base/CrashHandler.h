#pragma once
#include <Windows.h>

namespace KashipanEngine {

/// @brief CrashHandlerの登録
/// @param exception 例外ポインタ
/// @return 例外コード
LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);

} // namespace KashipanEngine