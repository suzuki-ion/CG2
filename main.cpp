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
    textures[0] = Texture::Load("Resources/uvChecker.png");

    // ウィンドウモード
    WindowMode windowMode = kWindow;
    // ブレンドモード
    BlendMode blendMode = kBlendModeNormal;

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
    // 球体
    //==================================================

    Sphere sphere(64);
    Object::StatePtr sphereState = sphere.GetStatePtr();
    sphereState.transform->translate.y = 6.0f;
    *sphereState.normalType = kNormalTypeVertex;
    *sphereState.fillMode = kFillModeWireframe;
    sphereState.material->color = { 64.0f, 64.0f, 64.0f, 255.0f, };
    sphere.SetRenderer(renderer);

    //==================================================
    // ICO球
    //==================================================

    Model icoSphere("Resources/ICOSphere", "icoSphere.obj");
    icoSphere.SetRenderer(renderer);
    icoSphere.GetStatePtr().transform->translate.y = 3.0f;
    *icoSphere.GetStatePtr().fillMode = kFillModeWireframe;

    //==================================================
    // モデル
    //==================================================

    Model model("Resources/Player", "player.obj");
    model.SetRenderer(renderer);

    //==================================================
    // 音声
    //==================================================

    int soundIndex = Sound::Load("C:\\Windows\\Media\\chord.wav");
    float volume = 1.0f;
    float pitch = 0.0f;
    bool isLoop = false;

    //==================================================
    // ボタン
    //==================================================

    // ボタン1
    Button button1("TestButton1", renderer);
    button1.SetUIElement<int>(
        "textureIndex",
        Texture::Load("Resources/tmpButton.png")
    );
    button1.SetUIElement("pos", Vector2(32.0f, 64.0f));
    
    // ボタン2
    Button button2("TestButton2", renderer);
    button2.SetUIElement<int>(
        "textureIndex",
        Texture::Load("Resources/tmpButton.png")
    );
    button2.SetUIElement("pos", Vector2(352.0f, 64.0f));

    //==================================================
    // UIグループ
    //==================================================

    UIGroup uiGroup("TestUIGroup", renderer);
    uiGroup.SetUIElement<int>(
        "textureIndex",
        Texture::Load("Resources/tmpUIGroup.png")
    );
    uiGroup.AddChild(&button1);
    uiGroup.AddChild(&button2);

    //==================================================
    // アニメーション用変数
    //==================================================

    KeyFrameAnimation keyFrameAnimation;
    KeyFrameElementData keyFrameElementData;

    keyFrameElementData.keyFrames.push_back({ 0.0f, -2.0f, Ease::InOutSine });
    keyFrameElementData.keyFrames.push_back({ 1.0f, +2.0f, Ease::InOutSine });
    keyFrameElementData.keyFrames.push_back({ 2.0f, -2.0f, Ease::InOutSine });
    keyFrameAnimation.SetKeyFrameElementData(KeyFrameElementType::kPositionX, keyFrameElementData);
    keyFrameElementData.keyFrames.clear();
    keyFrameElementData.keyFrames.push_back({ 0.0f, +0.0f, Ease::OutSine });
    keyFrameElementData.keyFrames.push_back({ 0.5f, +2.0f, Ease::InOutSine });
    keyFrameElementData.keyFrames.push_back({ 1.5f, -2.0f, Ease::InSine });
    keyFrameElementData.keyFrames.push_back({ 2.0f, +0.0f, Ease::OutSine });
    keyFrameAnimation.SetKeyFrameElementData(KeyFrameElementType::kPositionZ, keyFrameElementData);

    keyFrameAnimation.SetLoop(true);
    keyFrameAnimation.Play();

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
    // 線
    //==================================================

    int lineCount = 100;
    Lines lines(lineCount);
    lines.SetRenderer(renderer);

    //==================================================
    // スプライン曲線
    //==================================================

    std::vector<Vector3> points = {
        { 0.0f, 0.0f, 0.0f },
        { 10.0f, 10.0f, 0.0f },
        { 10.0f, 15.0f, 0.0f },
        { 20.0f, 15.0f, 0.0f },
        { 20.0f, 0.0f, 0.0f },
        { 30.0f, 0.0f, 0.0f }
    };
    float elapsedT = 1.0f / static_cast<float>(lineCount);
    // 線に適応
    auto linesStatePtr = lines.GetStatePtr();
    for (size_t i = 0; i <= lineCount; i++) {
        float t = elapsedT * static_cast<float>(i);
        Vector3 pos = Vector3::CatmullRomPosition(points, t);
        linesStatePtr.vertexData[i].pos = Vector4(pos);
    }

    std::ofstream file("test.obj");
    // 頂点座標 (v)
    for (int i = 0; i <= lineCount; ++i) {
        const Vector3 &v = Vector3(linesStatePtr.vertexData[i].pos);
        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    file.close();

    //==================================================
    // テキスト
    //==================================================

    Text text(1024);
    text.SetRenderer(renderer);
    text.SetFont("Resources/Font/test.fnt");
    char8_t textBuffer[1024] = u8"テストテキスト1";
    text.SetText(textBuffer);

    //==================================================
    // スクリーンバッファ
    //==================================================

    ScreenBuffer screenBuffer("TestScreen", 1920, 1080);
    screenBuffer.SetRenderer(renderer);

    //==================================================
    // キーコンフィグ
    //==================================================

    KeyConfig keyConfig;
    keyConfig.LoadFromJson("Resources/KeyConfig.json");

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop() == false) {
            continue;
        }
        screenBuffer.PreDraw();
        screenBuffer.PostDraw();
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

        ImGui::Begin("オブジェクト");

        // カメラ位置の表示
        ImGui::Text("カメラ位置: (%.2f, %.2f, %.2f)", camera->GetTranslate().x, camera->GetTranslate().y, camera->GetTranslate().z);
        // カメラの回転の表示
        ImGui::Text("カメラの回転: (%.2f, %.2f, %.2f)", camera->GetRotate().x, camera->GetRotate().y, camera->GetRotate().z);
        // ブレンドモードの表示
        ImGui::Text("ブレンドモード: %d", static_cast<int>(blendMode));
        // マウスの座標
        ImGui::Text("マウス座標: x.%d y.%d", static_cast<int>(Input::GetMouseX()), static_cast<int>(Input::GetMouseY()));
        
        // キーコンフィグのテスト用表示
        ImGui::Text("キーコンフィグテスト: %s", keyConfig.GetKeyConfig("MoveHorizontal").actionName.c_str());
        ImGui::Text("GetInputValue: %f", std::get<float>(keyConfig.GetInputValue("MoveHorizontal")));
        ImGui::Text("キーコンフィグテスト: %s", keyConfig.GetKeyConfig("Jump").actionName.c_str());
        ImGui::Text("GetInputValue: %d", std::get<bool>(keyConfig.GetInputValue("Jump")));

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

        // 球体
        if (ImGui::TreeNode("球体")) {
            ImGui::DragFloat3("Sphere Translate", &sphereState.transform->translate.x, 0.01f);
            ImGui::DragFloat3("Sphere Rotate", &sphereState.transform->rotate.x, 0.01f);
            ImGui::DragFloat3("Sphere Scale", &sphereState.transform->scale.x, 0.01f);
            ImGui::DragFloat4("Sphere MaterialColor", &sphereState.material->color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Sphere TextureIndex", sphereState.useTextureIndex);
            ImGui::TreePop();
        }

        // UIグループ
        if (ImGui::TreeNode("UIグループ")) {
            Vector2 pos = uiGroup.GetUIElement<Vector2>("pos");
            Vector2 scale = uiGroup.GetUIElement<Vector2>("scale");
            float degree = uiGroup.GetUIElement<float>("degree");
            Vector4 color = uiGroup.GetUIElement<Vector4>("color");
            ImGui::DragFloat2("UIGroup Translate", &pos.x, 1.0f);
            ImGui::DragFloat2("UIGroup Scale", &scale.x, 0.01f, 0.0f, 100.0f);
            ImGui::DragFloat("UIGroup Degree", &degree, 0.1f);
            ImGui::DragFloat4("UIGroup Color", &color.x, 1.0f, 0.0f, 255.0f);
            ImGui::TreePop();
            uiGroup.SetUIElement("pos", pos);
            uiGroup.SetUIElement("scale", scale);
            uiGroup.SetUIElement("degree", degree);
            uiGroup.SetUIElement("color", color);
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

        keyFrameAnimation.Update();
        icoSphere.GetStatePtr().transform->rotate.x += myGameEngine->GetDeltaTime();
        icoSphere.GetStatePtr().transform->rotate.y += myGameEngine->GetDeltaTime();
        icoSphere.GetStatePtr().transform->rotate.z += myGameEngine->GetDeltaTime();
        icoSphere.GetStatePtr().transform->translate.x =
            keyFrameAnimation.GetCurrentKeyFrameValue(KeyFrameElementType::kPositionX);
        icoSphere.GetStatePtr().transform->translate.z =
            keyFrameAnimation.GetCurrentKeyFrameValue(KeyFrameElementType::kPositionZ);

        uiGroup.Update();

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

        // スクリーンバッファの描画
        screenBuffer.DrawToImGui();

        // 線の描画
        lines.Draw();

        // 球体の描画
        sphere.Draw();
        // ICO球の描画
        icoSphere.Draw();
        // モデルの描画
        model.Draw();
        // ボタンの描画
        uiGroup.Draw();
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