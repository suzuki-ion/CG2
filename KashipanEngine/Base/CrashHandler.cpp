#include "CrashHandler.h"
#include <DbgHelp.h>
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")

namespace KashipanEngine {

LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception) {
    // 時刻を取得して、時刻を名前に入れたファイルを作成。Dumpディレクトリ以下に出力
    SYSTEMTIME time;
    GetLocalTime(&time);

    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    
    StringCchPrintfW(
        filePath, MAX_PATH,
        L"Dumps/%04d%02d%02d_%02d%02d%02d.dmp",
        time.wYear, time.wMonth, time.wDay,
        time.wHour, time.wMinute, time.wSecond
    );

    HANDLE dumpFileHandle = CreateFile(
        filePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        0,
        CREATE_ALWAYS,
        0,
        0
    );

    // processID （このexeのID）とクラッシュ（例外）の発生したthreadIDを取得
    DWORD processID = GetCurrentProcessId();
    DWORD threadID = GetCurrentThreadId();

    // 設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
    minidumpInformation.ThreadId = threadID;
    minidumpInformation.ExceptionPointers = exception;
    minidumpInformation.ClientPointers = TRUE;

    // Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
    MiniDumpWriteDump(
        GetCurrentProcess(),
        processID,
        dumpFileHandle,
        MiniDumpNormal,
        &minidumpInformation,
        nullptr,
        nullptr
    );

    // 他に関連づけられているSEH例外ハンドルがあれば実行。通常はプロセスを終了する
    return EXCEPTION_EXECUTE_HANDLER;
}

} // namespace KashipanEngine