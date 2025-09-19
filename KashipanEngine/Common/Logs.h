#pragma once
#include <string>
#include <source_location>

namespace KashipanEngine {

enum LogLevelFlags {
    kLogLevelFlagNone = 0b0000,
    kLogLevelFlagInfo = 0b0001,
    kLogLevelFlagWarning = 0b0010,
    kLogLevelFlagError = 0b0100,
    kLogLevelFlagAll = kLogLevelFlagInfo | kLogLevelFlagWarning | kLogLevelFlagError,
};

enum LogTypeFlags {
    kLogTypeFlagNone = 0b0000,
    kLogTypeFlagMessage = 0b0001,
    kLogTypeFlagLocation = 0b0010,
    kLogTypeFlagAll = kLogTypeFlagMessage | kLogTypeFlagLocation,
};

/// @brief ログ初期化
/// @param filePath ログファイルを保存するフォルダへのパス
/// @param projectDir プロジェクトのルートディレクトリへのパス
/// @param outputLogLevel 出力するログのレベル
/// @param outputLogType 出力するログの種類
void InitializeLog(
    const std::string &filePath,
    const std::string &projectDir,
    const LogLevelFlags outputLogLevel = kLogLevelFlagAll,
    const LogTypeFlags outputLogType = kLogTypeFlagAll
);

/// @brief 詳細情報有りのログ出力
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @param location ソースロケーション
void Log(
    const std::string &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo,
    const std::source_location &location = std::source_location::current()
);

/// @brief 詳細情報有りのログ出力
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @param location ソースロケーション
void Log(
    const std::wstring &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo,
    const std::source_location &location = std::source_location::current()
);

/// @brief 呼び出された場所のログ出力
/// @param message 呼び出された場所
/// @param logLevelFlags ログレベルフラグ
/// @param location ソースロケーション
void Log(
    const std::source_location &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo,
    const std::source_location &location = std::source_location::current()
);

/// @brief メッセージのみのログ出力
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
void LogSimple(
    const std::string &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo
);

/// @brief メッセージのみのログ出力
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
void LogSimple(
    const std::wstring &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo
);

/// @brief 呼び出された場所のみのログ出力
/// @param message 呼び出された場所
/// @param logLevelFlags ログレベルフラグ
void LogSimple(
    const std::source_location &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo
);

/// @brief ログ改行
void LogNewLine();

/// @brief ログ仕切り
/// @param partition 仕切り文字列
void LogInsertPartition(const std::string &partition = "\n==================================================\n");

} // namespace KashipanEngine