#pragma once
#include <Base/SceneManager.h>
#include <memory>

#include "3d/DirectionalLight.h"
#include "Math/Camera.h"
#include "Objects.h"

/// @brief テストシーンクラス
class TestScene : public KashipanEngine::SceneBase {
public:
    TestScene();
    ~TestScene() override = default;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

private:
    KashipanEngine::DirectionalLight directionalLight_;
    std::unique_ptr<KashipanEngine::Camera> camera_;
    std::unique_ptr<KashipanEngine::Text> titleText_;

    bool isDebugCameraActive_ = false;
    int frameRate_ = 60;
    static constexpr int kDefaultSubdivision = 16;
};