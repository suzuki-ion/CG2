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
    // DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon = myGameEngine->GetDxCommon();
    // レンダラーへのポインタ
    Renderer *renderer = myGameEngine->GetRenderer();

    // テクスチャを読み込む
    uint32_t textures[2];
    textures[0] = Texture::Load("Resources/uvChecker2.png");
    textures[1] = Texture::Load("Resources/testPlayer2.png");

    // ウィンドウモード
    WindowMode windowMode = kWindow;

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>(
        Vector3( 0.0f, 2.0f, -16.0f ),
        Vector3( 0.0f, 0.0f, 0.0f ),
        Vector3( 1.0f, 1.0f, 1.0f )
    );
    // デバッグカメラの有効化フラグ
    bool isUseDebugCamera = true;
    renderer->ToggleDebugCamera();
    // レンダラーにカメラを設定
    renderer->SetCamera(camera.get());

    //==================================================
    // 背景の色
    //==================================================

    Vector4 clearColor = { 32.0f, 32.0f, 32.0f, 255.0f };
    dxCommon->SetClearColor(ConvertColor(clearColor));

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight = {
        { 255.0f, 255.0f, 255.0f, 255.0f },
        { -0.5f, -0.75f, -0.5f },
        16.0f
    };

    //==================================================
    // キーコンフィグ
    //==================================================

    KeyConfig keyConfig;
    keyConfig.LoadFromJson("Resources/KeyConfig/PlayerKeyConfig.json");

    //==================================================
    // モデル
    //==================================================

    Model model("Resources/nahida", "nahida.obj");

    //==================================================
    // グリッド線
    //==================================================

    GridLine gridLineXZ(GridLineType::XZ, 1.0f, 10000);
    GridLine gridLineXY(GridLineType::XY, 1.0f, 10000);
    GridLine gridLineYZ(GridLineType::YZ, 1.0f, 10000);
    gridLineXZ.SetRenderer(renderer);
    gridLineXY.SetRenderer(renderer);
    gridLineYZ.SetRenderer(renderer);
    // XZグリッド描画フラグ
    bool isXZGrid = true;
    // XYグリッド描画フラグ
    bool isXYGrid = false;
    // YZグリッド描画フラグ
    bool isYZGrid = false;

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }

        //==================================================
        // 更新処理
        //==================================================

        // F3キーでデバッグカメラの有効化
        if (Input::IsKeyTrigger(DIK_F3)) {
            isUseDebugCamera = !isUseDebugCamera;
            renderer->ToggleDebugCamera();
        }

        // F11キーでウィンドウとフルスクリーン切り替え
        if (Input::IsKeyTrigger(DIK_F11)) {
            if (windowMode == kWindow) {
                windowMode = kFullScreenBorderLess;
            } else if (windowMode == kFullScreenBorderLess) {
                windowMode = kWindow;
            }
            winApp->SetWindowMode(windowMode);
        }

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

        // カメラ位置の表示
        ImGui::Text("カメラ位置: (%.2f, %.2f, %.2f)", camera->GetTranslate().x, camera->GetTranslate().y, camera->GetTranslate().z);
        // カメラの回転の表示
        ImGui::Text("カメラの回転: (%.2f, %.2f, %.2f)", camera->GetRotate().x, camera->GetRotate().y, camera->GetRotate().z);
        // マウスの座標
        ImGui::Text("マウス座標: x.%d y.%d", static_cast<int>(Input::GetMouseX()), static_cast<int>(Input::GetMouseY()));
        
        // キーコンフィグのテスト用表示
        ImGui::Text("キーコンフィグテスト: %s", keyConfig.GetKeyConfig("MoveX").actionName.c_str());
        ImGui::Text("GetInputValue: %f", std::get<float>(keyConfig.GetInputValue("MoveX")));
        ImGui::Text("キーコンフィグテスト: %s", keyConfig.GetKeyConfig("MoveY").actionName.c_str());
        ImGui::Text("GetInputValue: %f", std::get<float>(keyConfig.GetInputValue("MoveY")));
        ImGui::Text("キーコンフィグテスト: %s", keyConfig.GetKeyConfig("MoveZ").actionName.c_str());
        ImGui::Text("GetInputValue: %f", std::get<float>(keyConfig.GetInputValue("MoveZ")));

        // デバッグカメラの有効化
        if (ImGui::Checkbox("デバッグカメラ有効化", &isUseDebugCamera)) {
            renderer->ToggleDebugCamera();
        }

        // XZグリッドの描画フラグ
        ImGui::Checkbox("XZグリッド描画", &isXZGrid);
        // XYグリッドの描画フラグ
        ImGui::Checkbox("XYグリッド描画", &isXYGrid);
        // YZグリッドの描画フラグ
        ImGui::Checkbox("YZグリッド描画", &isYZGrid);

        // 背景色
        ImGui::DragFloat4("背景色", &clearColor.x, 1.0f, 0.0f, 255.0f);

        // 平行光源
        if (ImGui::TreeNode("平行光源")) {
            ImGui::DragFloat3("DirectionalLight Direction", &directionalLight.direction.x, 0.01f);
            ImGui::DragFloat4("DirectionalLight Color", &directionalLight.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::DragFloat("DirectionalLight Intensity", &directionalLight.intensity, 0.01f);
            ImGui::TreePop();
        }

        ImGui::End();
#else
#endif

        // モデルの移動
        if (std::get<float>(keyConfig.GetInputValue("MoveX")) != 0.0f) {
            auto statePtr = model.GetStatePtr();
            statePtr.transform->translate.x += std::get<float>(keyConfig.GetInputValue("MoveX")) * Engine::GetDeltaTime() * 10.0f;
        }
        if (std::get<float>(keyConfig.GetInputValue("MoveY"))) {
            auto statePtr = model.GetStatePtr();
            statePtr.transform->translate.y += std::get<float>(keyConfig.GetInputValue("MoveY")) * Engine::GetDeltaTime() * 10.0f;
        }
        if (std::get<float>(keyConfig.GetInputValue("MoveZ"))) {
            auto statePtr = model.GetStatePtr();
            statePtr.transform->translate.z += std::get<float>(keyConfig.GetInputValue("MoveZ")) * Engine::GetDeltaTime() * 10.0f;
        }

        //==================================================
        // 描画処理
        //==================================================

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));
        // 平行光源を設定
        renderer->SetLight(&directionalLight);

        // グリッド線の描画
        if (isXZGrid) gridLineXZ.Draw();
        if (isXYGrid) gridLineXY.Draw();
        if (isYZGrid) gridLineYZ.Draw();

        // モデルの描画
        model.Draw();

        myGameEngine->EndFrame();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}