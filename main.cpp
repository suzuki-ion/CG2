#include <KashipanEngine.h>
#include <Base/SceneManager.h>
#include <Base/Input.h>
#include <memory>
#include <imgui.h>

#include "GameProgram/TitleScene.h"
#include "GameProgram/GameScene.h"
#include "GameProgram/GameOverScene.h"

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    bool isDebug = false;
#ifdef _DEBUG
    isDebug = true;
#endif
    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("LE2A_08_スズキ_イオン_main() { Shoot(); }", 1920, 1080, isDebug);

    //==================================================
    // シーンの生成
    //==================================================

    // タイトルシーンの生成
    SceneManager::AddScene<TitleScene>("TitleScene");
    // ゲームシーンの生成
    SceneManager::AddScene<GameScene>("GameScene");
    // ゲームオーバーシーンの生成
    SceneManager::AddScene<GameOverScene>("GameOverScene");

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }
        Input::Update();

        // 更新処理
        SceneManager::UpdateActiveScene();
        // 描画処理
        SceneManager::DrawActiveScene();

        myGameEngine->EndFrame();
    }

    SceneManager::ClearScenes();
    return 0;
}