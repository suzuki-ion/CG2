#pragma once
#include "KashipanEngine.h"
#include <string>

namespace KashipanEngine {

class SceneBase {
public:
    static void Initialize(Engine *engine);

    SceneBase(const std::string &name);
    virtual ~SceneBase() = default;

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Update() = 0;
    virtual void Draw() = 0;

    bool IsInitialized() const { return isInitialized_; }
    const std::string &GetSceneName() const { return sceneName_; }

protected:
    Engine *engine_ = nullptr;
    bool isInitialized_ = false;

private:
    std::string sceneName_ = "SceneBase";
};

} // namespace KashipanEngine