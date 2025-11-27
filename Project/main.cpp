#include <KashipanEngine.h>
#include <memory>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Input.h"
#include "Base/SceneManager.h"
#include "Base/Renderer.h"
#include "2d/ImGuiManager.h"

#include "Application/Scenes/TitleScene.h"
#include "Application/Scenes/GameScene.h"
#include "Application/Scenes/ClearScene.h"

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("KashipanEngine", 1920, 1080, true);

    // WinAppクラスへのポインタ
    WinApp *winApp = myGameEngine->GetWinApp();
    winApp->SetSizeChangeMode(SizeChangeMode::kNormal);

    // シーン登録
    SceneManager::AddScene<TitleScene>("Title");
    SceneManager::AddScene<GameScene>("Game");
    SceneManager::AddScene<ClearScene>("Clear");

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    // デバッグカメラ切り替え用フラグ
    bool isDebugCameraActive = false;

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        //==================================================
        // 更新処理
        //==================================================

        // シーン更新 & 描画
        SceneManager::UpdateActiveScene();
        SceneManager::DrawActiveScene();

#ifdef USE_IMGUI
        ImGuiManager::Begin("KashipanEngine");
        // FPSの表示
        ImGui::Text("FPS: %d", myGameEngine->GetFPS());
        // デルタタイムの表示
        ImGui::Text("DeltaTime: %f", myGameEngine->GetDeltaTime());
        ImGui::InputInt("フレームレート", &frameRate);
        if (ImGui::Button("フレームレートを設定")) {
            myGameEngine->SetFrameRate(frameRate);
        }
        // デバッグカメラの有効化
        if (ImGui::Checkbox("デバッグカメラの有効化", &isDebugCameraActive)) {
            myGameEngine->GetRenderer()->ToggleDebugCamera();
        }
        ImGui::End();

        ImGui::Begin("入力テスト");
        ImGui::Text("マウス座標: x.%d y.%d", static_cast<int>(Input::GetMouseX()), static_cast<int>(Input::GetMouseY()));
        ImGui::End();
#else
        static_cast<void>(isDebugCameraActive);
#endif

        myGameEngine->EndFrame();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}