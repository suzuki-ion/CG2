#include <unordered_map>
#include "Common/Logs.h"
#include "SceneManager.h"

namespace KashipanEngine {
namespace {
std::vector<std::string> sSceneNames;
std::unordered_map<std::string, std::unique_ptr<SceneBase>> sScenes;
SceneBase *sActiveScene = nullptr;
} // namespace

void SceneManager::AddScene(const std::string &sceneName, std::unique_ptr<SceneBase> scene) {
    if (sScenes.find(sceneName) != sScenes.end()) {
        // 既に同名のシーンが存在する場合は警告
        Log("Scene with name '" + sceneName + "' already exists. Skipping addition.", kLogLevelFlagWarning);
        return;
    }

    // 既に初期化されている場合は一度終了処理を行う
    if (scene->IsInitialized()) {
        scene->Finalize();
    }
    sScenes[sceneName] = std::move(scene);
    sSceneNames.push_back(sceneName);

    // アクティブシーンがまだ設定されていない場合は、追加したシーンをアクティブにする
    if (sActiveScene == nullptr) {
        sActiveScene = sScenes[sceneName].get();
        sActiveScene->Initialize();
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

void SceneManager::ClearScenes() {
    sScenes.clear();
    sSceneNames.clear();
    sActiveScene = nullptr;
}

void SceneManager::SetActiveScene(const std::string &sceneName) {
    auto it = sScenes.find(sceneName);
    if (it != sScenes.end()) {
        if (sActiveScene->IsInitialized()) {
            sActiveScene->Finalize();
        }
        sActiveScene = it->second.get();
        if (!sActiveScene->IsInitialized()) {
            sActiveScene->Initialize();
        }

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
            if (pair.second.get() == sActiveScene) {
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