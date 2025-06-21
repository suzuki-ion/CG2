#include "TitleScene.h"
#include "GameScene.h"
#include "Base/Input.h"
#include "Base/Renderer.h"

using namespace KashipanEngine;

namespace {

// フレームレート
const int kFrameRate = 60;

// エンジンのインスタンスを作成
auto sKashipanEngine = std::make_unique<Engine>("KashipanEngine", 1920, 1080, true);
// レンダラーへのポインタ
Renderer *sRenderer = sKashipanEngine->GetRenderer();

// タイトルシーンのインスタンスを生成
std::unique_ptr<TitleScene> sTitleScene;
// ゲームシーンのインスタンスを生成
std::unique_ptr<GameScene> sGameScene;

// シーン
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

// 現在のシーン
Scene scene = Scene::kUnknown;

// シーン切り替え処理
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (sTitleScene->IsFinalized()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			sTitleScene.reset();
			// 新シーンの生成と初期化
			sGameScene = std::make_unique<GameScene>();
			sGameScene->Initialize(sKashipanEngine.get());
		}
		break;

	case Scene::kGame:
		if (sGameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			sGameScene->Finalize();
			sGameScene.reset();
			// 新シーンの生成と初期化
			sTitleScene = std::make_unique<TitleScene>();
			sTitleScene->Initialize(sKashipanEngine.get());
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		sTitleScene->Update();
		break;
	case Scene::kGame:
		sGameScene->Update();
		break;
	}
}

void DrawScene() {
	sRenderer->PreDraw();
	switch (scene) {
	case Scene::kTitle:
		sTitleScene->Draw();
		break;
	case Scene::kGame:
		sGameScene->Draw();
		break;
	}
	sRenderer->PostDraw();
}

} // namespace

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// 最初のシーンの初期化
	scene = Scene::kTitle;
	// タイトルシーンのインスタンスを生成
	sTitleScene = std::make_unique<TitleScene>();
	// タイトルシーンの初期化
	sTitleScene->Initialize(sKashipanEngine.get());

	// ウィンドウのxボタンが押されるまでループ
	while (sKashipanEngine->ProccessMessage() != -1) {
		sKashipanEngine->BeginFrame();
		if (sKashipanEngine->BeginGameLoop(kFrameRate) == false) {
			continue;
		}
		Input::Update();

		// シーン切り替え処理
		ChangeScene();

		// シーン更新処理
		UpdateScene();
		// シーン描画処理
		DrawScene();
	}

    sGameScene.reset();
    sTitleScene.reset();

	return 0;
}
