#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <filesystem>

#include "Common/VertexData.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;
class Renderer;

} // namespace KashipanEngine

/// @brief 自作エンジンクラス
class Engine final {
public:
    Engine(const char *title, int width = 1280, int height = 720, bool enableDebugLayer = true,
        const std::filesystem::path &projectDir = std::filesystem::current_path());
    ~Engine();

    /// @brief フレーム開始処理
    void BeginFrame();

    /// @brief ゲームループ開始処理
    /// @return ゲームループを開始するかどうか
    bool BeginGameLoop();

    /// @brief フレーム終了処理
    void EndFrame();

    /// @brief フレームレート設定
    /// @param frameRate フレームレート。最低24まで。無効な値(例: 24以下やモニターのFPS以上)の場合は垂直同期
    void SetFrameRate(int frameRate);

    /// @brief デルタタイム取得
    /// @return デルタタイム
    static float GetDeltaTime();

    /// @brief フレームレート取得
    /// @return フレームレート
    static unsigned int GetFPS();

    /// @brief WinAppクラスのポインタ取得
    /// @return WinAppクラスのポインタ
    KashipanEngine::WinApp *GetWinApp() const;

    /// @brief DirectXCommonクラスのポインタ取得
    /// @return DirectXCommonクラスのポインタ
    KashipanEngine::DirectXCommon *GetDxCommon() const;

    /// @brief レンダラーのポインタ取得
    /// @return レンダラーへのポインタ
    KashipanEngine::Renderer *GetRenderer() const;

    /// @brief メッセージ処理
    /// @return メッセージ処理結果。-1の場合は終了
    int ProccessMessage();

private:
};