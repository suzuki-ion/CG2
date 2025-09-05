#include <KashipanEngine.h>
#include <Base/WinApp.h>
#include <Base/Renderer.h>
#include <Base/Input.h>
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include <Objects/WorldTransform.h>
#include <Math/Camera.h>

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

    // WinAppを取得
    WinApp *winApp = myGameEngine->GetWinApp();
    // レンダラーを取得
    Renderer *renderer = myGameEngine->GetRenderer();
    // ウィンドウモード
    WindowMode windowMode = kWindow;

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera2D = std::make_unique<Camera>();
    camera2D->SetDimension(Camera::Dimension::k2D);
    CameraOrthographic cameraOrthographic;
    cameraOrthographic.left = 0.0f;
    cameraOrthographic.top = 1080.0f;
    cameraOrthographic.right = 1920.0f;
    cameraOrthographic.bottom = 0.0f;
    cameraOrthographic.nearClip = 0.0f;
    cameraOrthographic.farClip = 1.0f;
    camera2D->SetCameraOrthographic(cameraOrthographic);
    CameraViewport cameraViewport2D;
    cameraViewport2D.left = 0.0f;
    cameraViewport2D.top = 0.0f;
    cameraViewport2D.width = 1920.0f;
    cameraViewport2D.height = 1080.0f;
    cameraViewport2D.minDepth = 0.0f;
    cameraViewport2D.maxDepth = 1.0f;
    camera2D->SetCameraViewport(cameraViewport2D);

    // レンダラーにカメラを設定
    renderer->SetCamera(camera2D.get());

    Sprite sprite("Resources/testPlayer.png");
    sprite.SetPipelineName("Object2d.BlendNormal");
    sprite.SetIsUseCamera(true);
    WorldTransform worldTransform;
    worldTransform.translate_ = {
        1920.0f / 2.0f,
        1080.0f / 2.0f,
        0.0f
    };

    Text text(1024);
    text.SetFont("Resources/Font/test.fnt");
    char8_t textBuffer[1024] = u8"テストテキスト1";
    text.SetText(textBuffer);

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        //==================================================
        // 更新処理
        //==================================================

        // F11キーでウィンドウとフルスクリーン切り替え
        if (Input::IsKeyTrigger(DIK_F11)) {
            if (windowMode == kWindow) {
                windowMode = kFullScreenBorderLess;
            } else if (windowMode == kFullScreenBorderLess) {
                windowMode = kWindow;
            }
            winApp->SetWindowMode(windowMode);
        }

        if (Input::IsKeyDown(DIK_W)) {
            worldTransform.translate_.y += 4.0f;
        }
        if (Input::IsKeyDown(DIK_S)) {
            worldTransform.translate_.y -= 4.0f;
        }
        if (Input::IsKeyDown(DIK_A)) {
            worldTransform.translate_.x -= 4.0f;
        }
        if (Input::IsKeyDown(DIK_D)) {
            worldTransform.translate_.x += 4.0f;
        }

        ImGui::Begin("テキスト");
        ImGui::InputText("Text", reinterpret_cast<char *>(textBuffer), sizeof(textBuffer));
        ImGui::End();
        text.SetText(textBuffer);

        //==================================================
        // 描画処理
        //==================================================

        // スプライトの描画
        sprite.Draw(worldTransform);
        // 文字の描画
        text.Draw();

        myGameEngine->EndFrame();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}