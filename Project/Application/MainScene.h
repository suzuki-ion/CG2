#pragma once
#include <Base/SceneManager.h>
#include <memory>

#include "3d/DirectionalLight.h"
#include "Math/Camera.h"
#include "Objects.h"
#include "Objects/Particle.h"

/// @brief メインシーンクラス
class MainScene : public KashipanEngine::SceneBase {
public:
    MainScene();
    ~MainScene() override = default;
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

private:
    KashipanEngine::DirectionalLight directionalLight_;
    std::unique_ptr<KashipanEngine::Camera> camera_;
    KashipanEngine::Sphere sphere_;
    KashipanEngine::ParticleGroup *particleGroup_ = nullptr;

    bool isDebugCameraActive_ = false;
    int frameRate_ = 60;
    static constexpr int kDefaultSubdivision = 16;
};