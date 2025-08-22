#pragma once
#include <Common/SceneBase.h>

class GameScene : public KashipanEngine::SceneBase {
public:
    GameScene() = default;
    ~GameScene() override = default;

    void Update() override;
    void Draw() override;

private:
};