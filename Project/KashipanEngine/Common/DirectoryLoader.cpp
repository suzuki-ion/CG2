#include <filesystem>
#include <format>
#include <cassert>
#include "Common/Logs.h"
#include "DirectoryLoader.h"

namespace KashipanEngine {

namespace {

/// @brief 拡張子が指定されたリストに含まれているかをチェックする関数
/// @param fileName チェックするファイル名
/// @param extensions 拡張子のリスト
/// @return true: 拡張子がリストに含まれている, false: 含まれていない
bool HasValidExtension(const std::string &fileName, const std::vector<std::string> &extensions) {
    // 拡張子が指定されていない場合は常にtrueを返す
    if (extensions.empty()) {
        return true;
    }

    std::filesystem::path filePath(fileName);
    std::string extension = filePath.extension().string();
    
    for (const auto &ext : extensions) {
        if (extension == ext) {
            return true;
        }
    }
    return false;
}

/// @brief ファイル名push_back用関数
/// @param entry ファイル名
/// @param extensions 拡張子のリスト
/// @param files ファイル名を格納するベクター
void AddFileIfValid(const std::filesystem::directory_entry &entry, const std::vector<std::string> &extensions, std::vector<std::string> &files) {
    if (entry.is_regular_file()) {
        const std::string fileName = entry.path().string();
        if (HasValidExtension(fileName, extensions)) {
            files.push_back(fileName);
        }
    }
}

} // namespace

const std::vector<std::string> &GetFilesInDirectory(const std::string &directoryPath, const std::vector<std::string> &extensions, bool isRecursive) {
    static std::vector<std::string> files;
    files.clear();

    Log(std::format("Searching files in directory: '{}'", directoryPath));
    
    try {
        if (isRecursive) {
            for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
                AddFileIfValid(entry, extensions, files);
            }
        } else {
            for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
                AddFileIfValid(entry, extensions, files);
            }
        }

    } catch (const std::filesystem::filesystem_error &e) {
        LogSimple(std::format("Error accessing directory '{}': {}", directoryPath, e.what()), kLogLevelFlagError);
        assert(false && "Failed to access directory");
    }

    LogSimple(std::format("File search completed. file count: {}", files.size()));
    return files;
}

} // namespace KashipanEngine