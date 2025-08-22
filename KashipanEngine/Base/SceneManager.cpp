#include <unordered_map>
#include "Common/Logs.h"
#include "SceneManager.h"

namespace KashipanEngine {
namespace {
std::vector<std::string> sSceneNames;
std::unordered_map<std::string, SceneBase *> sScenes;
SceneBase *sActiveScene = nullptr;
} // namespace

void SceneManager::AddScene(const std::string &sceneName, SceneBase *scene) {
    if (sScenes.find(sceneName) != sScenes.end()) {
        // 既に同名のシーンが存在する場合は警告だけする
        Log("Scene with name '" + sceneName + "' already exists. Skipping addition.", kLogLevelFlagWarning);
        return;
    }
    sScenes[sceneName] = scene;
    sSceneNames.push_back(sceneName);

    // アクティブシーンがまだ設定されていない場合は、追加したシーンをアクティブにする
    if (sActiveScene == nullptr) {
        sActiveScene = scene;
    }
}

void SceneManager::RemoveScene(const std::string &sceneName) {
    auto it = sScenes.find(sceneName);
    if (it != sScenes.end()) {
        sSceneNames.erase(std::remove(sSceneNames.begin(), sSceneNames.end(), sceneName), sSceneNames.end());
        sScenes.erase(it);
    } else {
        Log("Scene with name '" + sceneName + "' does not exist. Cannot remove.", kLogLevelFlagWarning);
    }
}

void SceneManager::SetActiveScene(const std::string &sceneName) {
    auto it = sScenes.find(sceneName);
    if (it != sScenes.end()) {
        sActiveScene = it->second;
    } else {
        Log("Scene with name '" + sceneName + "' does not exist. Cannot set as active.", kLogLevelFlagWarning);
    }
}

void SceneManager::UpdateActiveScene() {
    if (sActiveScene) {
        sActiveScene->Update();
    } else {
        Log("No active scene set. Cannot update.", kLogLevelFlagWarning);
    }
}

void SceneManager::DrawActiveScene() {
    if (sActiveScene) {
        sActiveScene->Draw();
    } else {
        Log("No active scene set. Cannot draw.", kLogLevelFlagWarning);
    }
}

std::string SceneManager::GetActiveSceneName() {
    if (sActiveScene) {
        for (const auto &pair : sScenes) {
            if (pair.second == sActiveScene) {
                return pair.first;
            }
        }
    }
    // アクティブなシーンがない場合は警告を出して空文字列を返す
    Log("No active scene set. Returning empty string.", kLogLevelFlagWarning);
    return "";
}

std::vector<std::string> SceneManager::GetSceneNames() {
    return sSceneNames;
}

} // namespace KashipanEngine