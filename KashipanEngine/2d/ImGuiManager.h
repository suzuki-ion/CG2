#pragma once
#include <d3d12.h>
#include <imgui.h>
#include <wrl.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace KashipanEngine {

// 前方宣言
class WinApp;
class DirectXCommon;

class ImGuiManager final {
public:
    ImGuiManager(WinApp *winApp, DirectXCommon *dxCommon);
    ~ImGuiManager();

    /// @brief ImGuiのウィンドウ作成
    /// @param name ウィンドウの名前
    static void Begin(const char *name);

    /// @brief ImGuiのフレーム開始処理
    void BeginFrame();

    /// @brief ImGuiのフレーム終了処理
    void EndFrame();

    /// @brief ImGuiの再初期化
    void Reinitialize();

private:
    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;
};

} // namespace KashipanEngine