#pragma once
#include "Common/SceneBase.h"
#include "Objects/Plane.h"
#include "Objects/Particle.h"
#include "Math/Camera.h"
#include "Objects/Player.h"
#include <memory>

namespace KashipanEngine {

class CameraController; // 前方宣言

class GameScene : public SceneBase {
public:
    GameScene();
    ~GameScene() override = default;

    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

private:
    std::unique_ptr<Plane> ground_;
    std::unique_ptr<Player> player_;
    ParticleGroup *particleGroup_ = nullptr;
    int groundTextureIndex_ = 0;
    int playerTextureIndex_ = 0;
    int particleTextureIndex_ = 0;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<CameraController> cameraController_;
};

} // namespace KashipanEngine
