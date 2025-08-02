#include <KashipanEngine.h>
#include <memory>
#include <fstream>
#include <imgui.h>

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

#include "3d/DirectionalLight.h"
#include "Objects/Sphere.h"
#include "Objects/Model.h"
#include "Objects/Plane.h"
#include "Objects/Lines.h"
#include "Objects/Text.h"

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
    textures[0] = Texture::Load("Resources/uvChecker.png");
    textures[1] = Texture::Load("Resources/monsterBall.png");

    // ウィンドウモード
    WindowMode windowMode = kWindow;
    // ブレンドモード
    BlendMode blendMode = kBlendModeNormal;

    // フレームレート
    int frameRate = 60;
    myGameEngine->SetFrameRate(frameRate);

    //==================================================
    // オブジェクト管理用
    //==================================================

    std::vector<std::unique_ptr<Object>> objects;
    // 選択されてるオブジェクトのインデックス
    int selectedObjectIndex = 0;

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
        { -0.5f, -0.75f, 0.5f },
        1.0f
    };

    //==================================================
    // スプライト
    //==================================================

    std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>("Resources/uvChecker.png");
    sprite->SetRenderer(renderer);
    objects.emplace_back(std::move(sprite));

    //==================================================
    // 球体
    //==================================================

    std::unique_ptr<Sphere> sphere = nullptr;
    sphere = std::make_unique<Sphere>(64);
    Object::StatePtr sphereState = sphere->GetStatePtr();
    sphereState.transform->translate.y = 6.0f;
    *sphereState.normalType = kNormalTypeVertex;
    sphereState.material->color = { 64.0f, 64.0f, 64.0f, 255.0f, };
    sphere->SetRenderer(renderer);
    objects.emplace_back(std::move(sphere));

    //==================================================
    // モデル
    //==================================================

    std::unique_ptr<Model> model = nullptr;

    model = std::make_unique<Model>("Resources/nahida", "nahida.obj");
    model->SetRenderer(renderer);
    objects.emplace_back(std::move(model));

    model = std::make_unique<Model>("Resources", "multiMaterial.obj");
    model->SetRenderer(renderer);
    objects.emplace_back(std::move(model));

    model = std::make_unique<Model>("Resources", "bunny.obj");
    model->SetRenderer(renderer);
    objects.emplace_back(std::move(model));

    model = std::make_unique<Model>("Resources", "teapot.obj");
    model->SetRenderer(renderer);
    objects.emplace_back(std::move(model));

    model = std::make_unique<Model>("Resources", "suzanne.obj");
    model->SetRenderer(renderer);
    objects.emplace_back(std::move(model));

    //==================================================
    // 音声
    //==================================================

    int soundIndex = Sound::Load("C:\\Windows\\Media\\chord.wav");
    float volume = 1.0f;
    float pitch = 0.0f;
    bool isLoop = false;

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

    //==================================================
    // テキスト
    //==================================================

    Text text(1024);
    text.SetRenderer(renderer);
    text.SetFont("Resources/Font/test.fnt");
    char8_t textBuffer[1024] = u8"テストテキスト1";
    text.SetText(textBuffer);

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

        ImGuiManager::Begin("KashipanEngine");
        // FPSの表示
        ImGui::Text("FPS: %d", myGameEngine->GetFPS());
        // デルタタイムの表示
        ImGui::Text("DeltaTime: %f", myGameEngine->GetDeltaTime());
        ImGui::InputInt("フレームレート", &frameRate);
        if (ImGui::Button("フレームレートを設定")) {
            myGameEngine->SetFrameRate(frameRate);
        }
        ImGui::Combo("ブレンドモード", reinterpret_cast<int *>(&blendMode), "ブレンド無し\0通常\0加算\0減算\0乗算\0反転\0");
        ImGui::End();
        // ブレンドモードの設定
        renderer->SetBlendMode(blendMode);

        ImGui::Begin("入力テスト");
        ImGui::Text("コントローラー トリガー：\n\tLeft %d, Right %d",
            Input::GetXBoxLeftTrigger(), Input::GetXBoxRightTrigger());
        ImGui::Text("コントローラー スティック：\n\tLeft (%d, %d), Right (%d, %d)",
            Input::GetXBoxLeftStickX(), Input::GetXBoxLeftStickY(),
            Input::GetXBoxRightStickX(), Input::GetXBoxRightStickY());
        ImGui::Text("コントローラー ボタン：\n\tA %d, B %d, X %d, Y %d, START %d, BACK %d,\n\tLEFT_THUMB %d, RIGHT_THUMB %d\n\tLEFT_SHOULDER %d, RIGHT_SHOULDER %d\n\tUP %d, DOWN %d, LEFT %d, RIGHT %d",
            Input::IsXBoxButtonDown(XBoxButtonCode::A) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::B) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::X) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::Y) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::START) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::BACK) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::LEFT_THUMB) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::RIGHT_THUMB) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::LEFT_SHOULDER) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::RIGHT_SHOULDER) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::UP) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::DOWN) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::LEFT) ? 1 : 0,
            Input::IsXBoxButtonDown(XBoxButtonCode::RIGHT) ? 1 : 0);
        int xboxLeftMotor = Input::GetXBoxVibration(0, Input::LeftRightOption::Left);
        int xboxRightMotor = Input::GetXBoxVibration(0, Input::LeftRightOption::Right);
        ImGui::InputInt("XBox Left Motor", &xboxLeftMotor);
        ImGui::InputInt("XBox Right Motor", &xboxRightMotor);
        Input::SetXBoxVibration(0, xboxLeftMotor, xboxRightMotor);
        ImGui::End();

        ImGui::Begin("オブジェクト群");

        // カメラ位置の表示
        ImGui::Text("カメラ位置: (%.2f, %.2f, %.2f)", camera->GetTranslate().x, camera->GetTranslate().y, camera->GetTranslate().z);
        // カメラの回転の表示
        ImGui::Text("カメラの回転: (%.2f, %.2f, %.2f)", camera->GetRotate().x, camera->GetRotate().y, camera->GetRotate().z);
        // ブレンドモードの表示
        ImGui::Text("ブレンドモード: %d", static_cast<int>(blendMode));
        // マウスの座標
        ImGui::Text("マウス座標: x.%d y.%d", static_cast<int>(Input::GetMouseX()), static_cast<int>(Input::GetMouseY()));

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

        // 音声
        if (ImGui::TreeNode("音声")) {
            ImGui::DragFloat("音量", &volume, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("ピッチ", &pitch, 0.01f, -24.0f, 24.0f);
            ImGui::Checkbox("ループ再生", &isLoop);
            if (ImGui::Button("再生")) {
                Sound::Play(soundIndex, volume, pitch, isLoop);
            }
            if (ImGui::Button("停止")) {
                Sound::Stop(soundIndex);
            }
            ImGui::TreePop();
        }

        // 平行光源
        if (ImGui::TreeNode("平行光源")) {
            ImGui::DragFloat3("DirectionalLight Direction", &directionalLight.direction.x, 0.01f);
            ImGui::DragFloat4("DirectionalLight Color", &directionalLight.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::DragFloat("DirectionalLight Intensity", &directionalLight.intensity, 0.01f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("オブジェクト")) {
            if (ImGui::BeginCombo("オブジェクト選択", objects[selectedObjectIndex]->GetName().c_str())) {
                for (size_t i = 0; i < objects.size(); ++i) {
                    if (ImGui::Selectable(objects[i]->GetName().c_str(), selectedObjectIndex == static_cast<int>(i))) {
                        selectedObjectIndex = static_cast<int>(i);
                    }
                }
                ImGui::EndCombo();
            }

            // 選択されてるオブジェクトの状態を表示
            if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(objects.size())) {
                Object::StatePtr state = objects[selectedObjectIndex]->GetStatePtr();
                ImGui::DragFloat3("Translate", &state.transform->translate.x, 0.01f);
                ImGui::DragFloat3("Rotate", &state.transform->rotate.x, 0.01f);
                ImGui::DragFloat3("Scale", &state.transform->scale.x, 0.01f);
                ImGui::DragFloat4("MaterialColor", &state.material->color.x, 1.0f, 0.0f, 255.0f);
                ImGui::InputInt("LightingType", &state.material->lightingType);
                // テクスチャインデックスへのポインタがあれば表示
                if (state.useTextureIndex) {
                    ImGui::InputInt("Use Texture Index", state.useTextureIndex);
                } else {
                    ImGui::Text("Use Texture Index: Access Privileges.");
                }
                // UVTransformへのポインタもあれば表示
                if (state.uvTransform) {
                    if (ImGui::TreeNode("UVTransform")) {
                        ImGui::DragFloat2("UVTranslate", &state.uvTransform->translate.x, 0.01f);
                        ImGui::DragFloat2("UVScale", &state.uvTransform->scale.x, 0.01f);
                        ImGui::DragFloat2("UVRotate", &state.uvTransform->rotate.x, 0.01f);
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }

        // テキスト
        if (ImGui::TreeNode("テキスト")) {
            ImGui::InputTextMultiline("テキスト入力", reinterpret_cast<char *>(textBuffer), IM_ARRAYSIZE(textBuffer),
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16.0f));
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                text.SetText(textBuffer);
            }
            ImGui::TreePop();
            text.SetText(textBuffer);
        }

        ImGui::End();

        //==================================================
        // 描画処理
        //==================================================

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));
        // 平行光源を設定
        renderer->SetLight(&directionalLight);

        for (const auto &object : objects) {
            // オブジェクトの描画
            object->Draw();
        }

        // 文字の描画
        text.Draw();
        
        renderer->PostDraw();
        myGameEngine->EndFrame();

        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}