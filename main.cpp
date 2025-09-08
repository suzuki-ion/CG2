#include <KashipanEngine.h>
#include <Base/WinApp.h>
#include <Base/Input.h>
#include <Base/SceneManager.h>

#include "GameScene.h"

#include <memory>
#include <imgui.h>

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("KashipanEngine", 1920, 1080, true);
    WinApp *winApp = myGameEngine->GetWinApp();
    WindowMode windowMode = kWindow;
    winApp->SetWindowMode(windowMode);

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    //==================================================
    // シーン登録
    //==================================================

    SceneManager::AddScene<GameScene>("GameScene");

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        // F11キーでウィンドウとフルスクリーン切り替え(リリース時のみ)
#ifdef _DEBUG
#else
        if (Input::IsKeyTrigger(DIK_F11)) {
            if (windowMode == kWindow) {
                windowMode = kFullScreenBorderLess;
            } else if (windowMode == kFullScreenBorderLess) {
                windowMode = kWindow;
            }
            winApp->SetWindowMode(windowMode);
        }
#endif
        SceneManager::UpdateActiveScene();
        SceneManager::DrawActiveScene();

        myGameEngine->EndFrame();
    }

    return 0;
}