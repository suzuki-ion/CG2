#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Common/SceneBase.h"

namespace KashipanEngine {

class SceneManager {
public:
    SceneManager() = delete;
    ~SceneManager() = delete;
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;
    SceneManager(SceneManager &&) = delete;
    SceneManager &operator=(SceneManager &&) = delete;

    static void AddScene(const std::string &sceneName, std::unique_ptr<SceneBase> scene);

    static void RemoveScene(const std::string &sceneName);
    static void ClearScenes();
    static void SetActiveScene(const std::string &sceneName);

    static void UpdateActiveScene();
    static void DrawActiveScene();

    static std::string GetActiveSceneName();
    static std::vector<std::string> GetSceneNames();
};

} // namespace KashipanEngine