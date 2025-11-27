#pragma once
#include "Common/SceneBase.h"
#include "Base/SceneManager.h"
#include "Base/Input.h"
#include "Common/Logs.h"
#include "Math/Camera.h"
#include "Base/WinApp.h"
#include "Base/Renderer.h"

namespace KashipanEngine {

class ClearScene : public SceneBase {
public:
    ClearScene() : SceneBase("Clear") {}
    ~ClearScene() override = default;

    void Initialize() override {
        if (isInitialized_) { return; }
        Log("ClearScene Initialize.");
        if (!camera_) {
            auto *winApp = engine_->GetWinApp();
            int width = winApp ? winApp->GetClientWidth() : 1280;
            int height = winApp ? winApp->GetClientHeight() : 720;
            camera_ = std::make_unique<Camera>(Vector3(0.0f, 2.0f, -8.0f), Vector3(0.1f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
            CameraPerspective persp{};
            persp.fovY = 0.45f;
            persp.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            persp.nearClip = 0.1f;
            persp.farClip = 100.0f;
            camera_->SetCameraPerspective(persp);
            CameraViewport vp{};
            vp.left = 0.0f; vp.top = 0.0f; vp.width = static_cast<float>(width); vp.height = static_cast<float>(height); vp.minDepth = 0.0f; vp.maxDepth = 1.0f;
            camera_->SetCameraViewport(vp);
            engine_->GetRenderer()->SetCamera(camera_.get());
        }
        isInitialized_ = true;
    }
    void Finalize() override {
        if (!isInitialized_) { return; }
        Log("ClearScene Finalize.");
        camera_.reset();
        isInitialized_ = false;
    }
    void Update() override {
        if (Input::IsKeyTrigger(DIK_SPACE) || Input::IsKeyTrigger(DIK_E)) {
            SceneManager::SetActiveScene("Title");
        }
    }
    void Draw() override {
    }
private:
    std::unique_ptr<Camera> camera_;
};

} // namespace KashipanEngine
