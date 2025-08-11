#pragma once
#include <string>
#include <vector>

namespace KashipanEngine {

/// @brief 指定のディレクトリ内のファイル名を取得する
/// @param directoryPath ディレクトリのパス
/// @param extensions ファイル拡張子のリスト。空の場合は全てのファイルを対象とする
/// @param isRecursive 再帰的にサブディレクトリも探索するかどうか
/// @return 指定ディレクトリ内のファイル名のリスト
const std::vector<std::string> &GetFilesInDirectory(
    const std::string &directoryPath,
    const std::vector<std::string> &extensions = {},
    bool isRecursive = false
);

} // namespace KashipanEngine