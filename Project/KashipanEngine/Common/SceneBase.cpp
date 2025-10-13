#include "Base/SceneManager.h"
#include "SceneBase.h"

namespace KashipanEngine {
namespace {
Engine *sEngine = nullptr;
} // namespace

void SceneBase::Initialize(Engine *engine) {
    sEngine = engine;
}

SceneBase::SceneBase(const std::string &name) {
    engine_ = sEngine;
    sceneName_ = name;
}

} // namespace KashipanEngine