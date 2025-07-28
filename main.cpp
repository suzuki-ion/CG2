#include <KashipanEngine.h>
#include <Base/Renderer.h>
#include <Base/Input.h>

using namespace KashipanEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine =
        std::make_unique<Engine>("LE2A_08_スズキ_イオン_AL3", 1920, 1080, false);
    // レンダラーへのポインタ
    Renderer *renderer = myGameEngine->GetRenderer();

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }
        renderer->PreDraw();
        Input::Update();

        //==================================================
        // 更新処理
        //==================================================



        //==================================================
        // 描画処理
        //==================================================


        
        renderer->PostDraw();
        myGameEngine->EndFrame();

        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}