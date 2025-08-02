#include <Windows.h>
#include <filesystem>
#include <fstream>
#include "Logs.h"
#include "TimeGet.h"
#include "ConvertString.h"

namespace KashipanEngine {

// ログはデバッグ時のみ出力する
#ifdef _DEBUG
namespace {

// ログ出力用のストリーム
std::ofstream sLogStream;
// プロジェクトのルートディレクトリ
std::string sProjectDir;
// 出力するログのレベル
LogLevelFlags sOutputLogLevel;
// 出力するログの種類
LogTypeFlags sOutputLogType;
// ログレベルフラグの文字列
const char *kLogLevelFlagStrings[] = {
    "NONE",
    "INFO",
    "WARNING",
    "ERROR",
};

/// @brief 相対パスを取得
/// @param fullPath フルパス
/// @return 相対パス
std::string GetRelativePath(const std::string &fullPath) {
    if (fullPath.find(sProjectDir) == 0) {
        return fullPath.substr(sProjectDir.length());
    }
    return fullPath;
}

/// @brief ログ出力用の詳細情報テキストを作成
/// @param location ソースロケーション
/// @return 詳細情報テキスト
std::string CreateDetailLogText(const std::source_location &location) {
    std::string logText;
    logText += TimeGetString("[ {:%Y/%m/%d %H:%M:%S} ]\n\t");

    //--------- File ---------//
    logText += "File: \"";
    logText += GetRelativePath(location.file_name());
    logText += "\"\n\t";

    //--------- Function ---------//
    logText += "Function: \"";
    logText += location.function_name();
    logText += "\"\n\t";

    //--------- Line ---------//
    logText += "Line: ";
    logText += std::to_string(location.line());
    logText += "\n\t";

    return logText;
}

/// @brief ログ出力用の詳細情報無しのテキストを作成
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @return ログ出力用のテキスト
std::string CreateLogText(const std::string &message, LogLevelFlags logLevelFlags) {
    std::string logText;

    //--------- Message ---------//
    logText += "Message: ";
    if (logLevelFlags & kLogLevelFlagInfo) {
        logText += "[INFO] ";
    } else if (logLevelFlags & kLogLevelFlagWarning) {
        logText += "[--- WARNING ---] ";
    } else if (logLevelFlags & kLogLevelFlagError) {
        logText += "[!!!--- ERROR ---!!!] ";
    } else {
        logText += "[NO LEVEL LOG] ";
    }
    logText += message;

    return logText;
}

} // namespace

void InitializeLog(const std::string &filePath, const std::string &projectDir,
    const LogLevelFlags outputLogLevel, const LogTypeFlags outputLogType) {
    // プロジェクトのルートディレクトリを保存
    sProjectDir = projectDir;

    // ログファイルのパスを作成
    std::filesystem::create_directory(filePath);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = filePath + '/' + TimeGetString("{:%Y-%m-%d_%H-%M-%S}") + ".log";
    // ファイルを使って書き込み準備
    sLogStream.open(logFilePath);

    // 出力するログのレベルを保存
    sOutputLogLevel = outputLogLevel;
    // 出力するログの種類を保存
    sOutputLogType = outputLogType;

    // 初期化完了のログとプロジェクトのルートディレクトリと
    // 出力するログのレベルと種類を出力
    sLogStream << "Log initialized." << std::endl;
    sLogStream << "Project Directory: \"" << projectDir << "\"" << std::endl;
    if (outputLogLevel & kLogLevelFlagInfo) {
        sLogStream << "Output Log Level: [INFO]" << std::endl;
    }
    if (outputLogLevel & kLogLevelFlagWarning) {
        sLogStream << "Output Log Level: [WARNING]" << std::endl;
    }
    if (outputLogLevel & kLogLevelFlagError) {
        sLogStream << "Output Log Level: [ERROR]" << std::endl;
    }
    sLogStream << std::endl;
    if (outputLogType & kLogTypeFlagMessage) {
        sLogStream << "Output Log Type: [MESSAGE]" << std::endl;
    }
    if (outputLogType & kLogTypeFlagLocation) {
        sLogStream << "Output Log Type: [LOCATION]" << std::endl;
    }

    // ビルドがDebugかReleaseかを出力
#ifdef _DEBUG
    sLogStream << "Build Type: [DEBUG]" << std::endl;
#else
    sLogStream << "Build Type: [RELEASE]" << std::endl;
#endif

    sLogStream << std::endl;
}

void Log(const std::string &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = CreateDetailLogText(location);
    logText += CreateLogText(message, logLevelFlags);
    // ログファイルに書き込み
    sLogStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void Log(const std::wstring &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // 処理を共通化したいので元あるLog関数を使用
    Log(ConvertString(message), logLevelFlags, location);
}

void Log(const std::source_location &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = CreateDetailLogText(location);
    logText += CreateLogText(
        "[Location] [File:\"" +
        GetRelativePath(message.file_name()) +
        "\" Function:\"" +
        message.function_name() +
        "\" Line:" +
        std::to_string(message.line()) +
        "]",
        logLevelFlags
    );
    // ログファイルに書き込み
    sLogStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogSimple(const std::string &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = '\t' + CreateLogText(message, logLevelFlags);
    // ログファイルに書き込み
    sLogStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogSimple(const std::wstring &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // 処理を共通化したいので元あるLog関数を使用
    LogSimple(ConvertString(message), logLevelFlags);
}

void LogSimple(const std::source_location &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & sOutputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = '\t' + CreateLogText(
        "[Location] [File:\"" +
        GetRelativePath(message.file_name()) +
        "\" Function:\"" +
        message.function_name() +
        "\" Line:" +
        std::to_string(message.line()) +
        "]",
        logLevelFlags
    );
    // ログファイルに書き込み
    sLogStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogNewLine() {
    sLogStream << std::endl;
    OutputDebugStringA("\n");
}

void LogInsertPartition(const std::string &partition) {
    sLogStream << partition << std::endl;
    OutputDebugStringA((partition + "\n").c_str());
}
#else

void InitializeLog(const std::string &filePath, const std::string &projectDir,
    const LogLevelFlags outputLogLevel, const LogTypeFlags outputLogType) {
    static_cast<void>(filePath);
    static_cast<void>(projectDir);
    static_cast<void>(outputLogLevel);
    static_cast<void>(outputLogType);
}

void Log(const std::string &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
    static_cast<void>(location);
}

void Log(const std::wstring &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
    static_cast<void>(location);
}

void Log(const std::source_location &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
    static_cast<void>(location);
}

void LogSimple(const std::string &message, const LogLevelFlags logLevelFlags) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
}

void LogSimple(const std::wstring &message, const LogLevelFlags logLevelFlags) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
}

void LogSimple(const std::source_location &message, const LogLevelFlags logLevelFlags) {
    static_cast<void>(message);
    static_cast<void>(logLevelFlags);
}

void LogNewLine() {
}

void LogInsertPartition(const std::string &partition) {
    static_cast<void>(partition);
}
#endif // _DEBUG

} // namespace KashipanEngine