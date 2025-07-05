#include <KashipanEngine.h>
#include <memory>
#include <imgui.h>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Renderer.h"
#include "Base/Texture.h"
#include "Base/Input.h"
#include "Base/Sound.h"

#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Common/ConvertColor.h"
#include "Common/KeyFrameAnimation.h"

#include "3d/DirectionalLight.h"
#include "Objects/Sphere.h"
#include "Objects/Model.h"
#include "Objects/Plane.h"
#include "Objects/BillBoard.h"

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
        { -0.5f, 0.75f, -0.5f },
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
    for (auto &model : icoSphere.GetModels()) {
        model.GetStatePtr().transform->translate.y = 3.0f;
        *model.GetStatePtr().fillMode = kFillModeWireframe;
    }

    //==================================================
    // モデル
    //==================================================

    Model model("Resources/nahida", "nahida.obj");
    model.SetRenderer(renderer);
    for (auto &modelElement : model.GetModels()) {
        *modelElement.GetStatePtr().fillMode = kFillModeWireframe;
    }

    //==================================================
    // 床用の板
    //==================================================

    Plane floor;
    Object::StatePtr floorState = floor.GetStatePtr();
    // 左前
    floorState.mesh->vertexBufferMap[0].position = { -5.0f, 0.0f, -5.0f, 1.0f };
    floorState.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 左奥
    floorState.mesh->vertexBufferMap[1].position = { -5.0f, 0.0f, 5.0f, 1.0f };
    floorState.mesh->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    // 右前
    floorState.mesh->vertexBufferMap[2].position = { 5.0f, 0.0f, -5.0f, 1.0f };
    floorState.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // 右奥
    floorState.mesh->vertexBufferMap[3].position = { 5.0f, 0.0f, 5.0f, 1.0f };
    floorState.mesh->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
    floorState.material->enableLighting = true;
    // テクスチャを設定
    *floorState.useTextureIndex = textures[0];
    // 法線の種類
    *floorState.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    *floorState.fillMode = kFillModeSolid;
    floor.SetRenderer(renderer);

    //==================================================
    // ビルボード
    //==================================================

    BillBoard billBoard(camera->GetRotatePtr());
    billBoard.SetRenderer(renderer);

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

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        if (myGameEngine->BeginGameLoop(frameRate) == false) {
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
        ImGui::Combo("ブレンドモード", reinterpret_cast<int *>(&blendMode), "ブレンド無し\0通常\0加算\0減算\0乗算\0反転\0");
        ImGui::End();
        // ブレンドモードの設定
        renderer->SetBlendMode(blendMode);

        ImGuiManager::Begin("オブジェクト");

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

        // 板
        if (ImGui::TreeNode("板")) {
            ImGui::DragFloat3("Plane Translate", &floorState.transform->translate.x, 0.01f);
            ImGui::DragFloat3("Plane Rotate", &floorState.transform->rotate.x, 0.01f);
            ImGui::DragFloat3("Plane Scale", &floorState.transform->scale.x, 0.01f);
            ImGui::DragFloat4("Plane MaterialColor", &floorState.material->color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Plane TextureIndex", floorState.useTextureIndex);
            if (ImGui::TreeNode("Plane uvTransform")) {
                ImGui::DragFloat2("Plane uvTransform Translate", &floorState.uvTransform->translate.x, 0.01f);
                ImGui::DragFloat3("Plane uvTransform Rotate", &floorState.uvTransform->rotate.x, 0.01f);
                ImGui::DragFloat2("Plane uvTransform Scale", &floorState.uvTransform->scale.x, 0.01f);
                ImGui::TreePop();
            }
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

        ImGui::End();

        keyFrameAnimation.Update();
        for (auto &icoShpereModel : icoSphere.GetModels()) {
            icoShpereModel.GetStatePtr().transform->rotate.x += myGameEngine->GetDeltaTime();
            icoShpereModel.GetStatePtr().transform->rotate.y += myGameEngine->GetDeltaTime();
            icoShpereModel.GetStatePtr().transform->rotate.z += myGameEngine->GetDeltaTime();
            icoShpereModel.GetStatePtr().transform->translate.x =
                keyFrameAnimation.GetCurrentKeyFrameValue(KeyFrameElementType::kPositionX);
            icoShpereModel.GetStatePtr().transform->translate.z =
                keyFrameAnimation.GetCurrentKeyFrameValue(KeyFrameElementType::kPositionZ);
        }

        uiGroup.Update();

        //==================================================
        // 描画処理
        //==================================================

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));
        // 平行光源を設定
        renderer->SetLight(&directionalLight);

        // 球体の描画
        sphere.Draw();
        // ICO球の描画
        icoSphere.Draw();
        // モデルの描画
        model.Draw();
        // 板の描画
        floor.Draw();
        // ボタンの描画
        uiGroup.Draw();
        
        renderer->PostDraw();
        myGameEngine->EndFrame();

        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}