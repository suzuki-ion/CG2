#include <KashipanEngine.h>
#include <memory>
#include <fstream>
#if DEBUG_BUILD || DEVELOP_BUILD
#include <imgui.h>
#endif

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Renderer.h"
#include "Base/Texture.h"
#include "Base/Input.h"
#include "Base/Sound.h"
#include "Base/ScreenBuffer.h"

#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Common/ConvertColor.h"
#include "Common/KeyFrameAnimation.h"
#include "Common/GridLine.h"
#include "Common/KeyConfig.h"

#include "3d/DirectionalLight.h"
#include "Objects.h"

#include "2d/UI/GUI/Button.h"
#include "2d/UI/UIGroup.h"

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

    // テクスチャを読み込む
    uint32_t textures[2];
    textures[0] = Texture::Load("Resources/uvChecker.png");
    textures[1] = Texture::Load("Resources/testPlayer.png");

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    // デバッグカメラ切り替え用フラグ
    bool isDebugCameraActive = false;

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight{};
    directionalLight.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    directionalLight.direction = Vector3(0.5f, -0.75f, -0.5f).Normalize();
    directionalLight.intensity = 16.0f;

    //==================================================
    // モデル
    //==================================================

    // モデルに使用するカメラ
    std::unique_ptr<Camera> camera = std::make_unique<Camera>();
    camera->SetTranslate({ 0.0f, 1.0f, -16.0f });
    // カメラをレンダラーにセット
    myGameEngine->GetRenderer()->SetCamera(camera.get());

    // モデルコンテナ
    std::vector<std::unique_ptr<Model>> models;
    // モデルの生成
    models.push_back(std::make_unique<Model>("Resources/nahida", "nahida.obj"));
    models.back()->GetStatePtr().transform->translate = { 0.0f, 0.0f, 0.0f };
    models.push_back(std::make_unique<Model>("Resources/Ground", "ground.obj"));
    models.back()->GetStatePtr().transform->translate = { 0.0f, 0.0f, 0.0f };

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        //==================================================
        // 更新処理
        //==================================================

        // 平行光源をレンダラーにセット
        myGameEngine->GetRenderer()->SetLight(&directionalLight);
#if !RELEASE_BUILD
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
        ImGui::Text("コントローラー トリガー：\n\tLeft %d, Right %d",
            Input::GetXBoxLeftTrigger(), Input::GetXBoxRightTrigger());
        ImGui::Text("コントローラー スティック：\n\tLeft (%d, %d), Right (%d, %d)",
            Input::GetXBoxLeftStickX(), Input::GetXBoxLeftStickY(),
            Input::GetXBoxRightStickX(), Input::GetXBoxRightStickY());
        ImGui::Text("コントローラー ボタン：\n\tA %d, B %d, X %d, Y %d, START %d, BACK %d,\n\tLEFT_THUMB %d, RIGHT_THUMB %d\n\tLEFT_SHOULDER %d, RIGHT_SHOULDER %d\n\tUP %d, DOWN %d, LEFT %d, RIGHT %d",
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::A)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::B)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::X)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::Y)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::START)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::BACK)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::LEFT_THUMB)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::RIGHT_THUMB)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::LEFT_SHOULDER)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::RIGHT_SHOULDER)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::UP)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::DOWN)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::LEFT)) ? 1 : 0,
            Input::IsXBoxButtonDown(static_cast<int>(XBoxButtonCode::RIGHT)) ? 1 : 0);
        int xboxLeftMotor = Input::GetXBoxVibration(0, Input::LeftRightOption::Left);
        int xboxRightMotor = Input::GetXBoxVibration(0, Input::LeftRightOption::Right);
        ImGui::InputInt("XBox Left Motor", &xboxLeftMotor);
        ImGui::InputInt("XBox Right Motor", &xboxRightMotor);
        Input::SetXBoxVibration(0, xboxLeftMotor, xboxRightMotor);
        ImGui::End();

        ImGui::Begin("オブジェクト");

        // マウスの座標
        ImGui::Text("マウス座標: x.%d y.%d", static_cast<int>(Input::GetMouseX()), static_cast<int>(Input::GetMouseY()));

        ImGui::End();

        // モデル
        ImGui::Begin("モデル");
        for (size_t i = 0; i < models.size(); i++) {
            auto modelState = models[i]->GetStatePtr();
            ImGui::Text("モデル %zu", i);
            ImGui::InputFloat3(("位置##model" + std::to_string(i)).c_str(), &modelState.transform->translate.x);
            ImGui::InputFloat3(("回転##model" + std::to_string(i)).c_str(), &modelState.transform->rotate.x);
            ImGui::InputFloat3(("拡縮##model" + std::to_string(i)).c_str(), &modelState.transform->scale.x);
        }
        ImGui::End();
#else
        static_cast<void>(isDebugCameraActive);
#endif

        //==================================================
        // 描画処理
        //==================================================

        for (size_t i = 0; i < models.size(); i++) {
            models[i]->Draw();
        }

        myGameEngine->EndFrame();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}