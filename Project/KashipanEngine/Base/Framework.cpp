#include "Framework.h"
#include "KashipanEngine.h"
#include "Base/Input.h"
#include "Base/SceneFactory.h"
#include "../../Application/MainScene.h"
#include "../../Application/TestScene.h"

using namespace KashipanEngine;

Framework::Framework() = default;
Framework::~Framework() = default;

void Framework::Initialize(const char *title, int width, int height, bool enableDebugLayer) {
    engine_ = std::make_unique<Engine>(title, width, height, enableDebugLayer);

    SceneFactory::AddScene<TestScene>("TestScene");
    SceneFactory::AddScene<MainScene>("MainScene");
}

void Framework::Run() {
    while (engine_->ProccessMessage() != -1) {
        engine_->BeginFrame();
        if (engine_->BeginGameLoop() == false) {
            continue;
        }

        KashipanEngine::SceneManager::UpdateActiveScene();
        KashipanEngine::SceneManager::DrawActiveScene();

        engine_->EndFrame();
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }
}

void Framework::Finalize() {
}
