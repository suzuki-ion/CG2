#pragma once
#include <string>

namespace KashipanEngine {

class SceneBase {
public:
    SceneBase(const std::string &name);
    virtual ~SceneBase() = default;

    virtual void Update() = 0;
    virtual void Draw() = 0;
    const std::string &GetSceneName() const { return sceneName_; }

private:
    std::string sceneName_;
};

} // namespace KashipanEngine