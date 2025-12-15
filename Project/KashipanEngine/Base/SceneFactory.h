#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <type_traits>

#include "Common/SceneBase.h"
#include "Base/SceneManager.h"

namespace KashipanEngine {

class SceneFactory {
public:
    SceneFactory() = delete;

    template<typename T, typename... Args>
    static void AddScene(const std::string &sceneName, Args&&... args) {
        static_assert(std::is_base_of_v<SceneBase, T>, "T must be derived from SceneBase");
        SceneManager::AddScene(sceneName, std::make_unique<T>(std::forward<Args>(args)...));
    }

    using CreatorFn = std::function<std::unique_ptr<SceneBase>()>;

    static void RegisterCreator(const std::string &sceneName, CreatorFn creator) {
        Registry()[sceneName] = std::move(creator);
    }

    static std::unique_ptr<SceneBase> Create(const std::string &sceneName) {
        auto &r = Registry();
        auto it = r.find(sceneName);
        if (it == r.end()) return nullptr;
        return it->second();
    }

private:
    static std::unordered_map<std::string, CreatorFn> &Registry() {
        static std::unordered_map<std::string, CreatorFn> registry;
        return registry;
    }
};

} // namespace KashipanEngine
