#pragma once
#include <string>
#include <vector>
#include "Common/SceneBase.h"

namespace KashipanEngine {

class SceneManager {
public:
    static void Initialize();

    static void AddScene(const std::string &sceneName, SceneBase *scene);
    static void RemoveScene(const std::string &sceneName);
    static void SetActiveScene(const std::string &sceneName);

    static void UpdateActiveScene();
    static void DrawActiveScene();

    static std::string GetActiveSceneName();
    static std::vector<std::string> GetSceneNames();
};

} // namespace KashipanEngine