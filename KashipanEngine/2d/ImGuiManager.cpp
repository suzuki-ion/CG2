#include "ImGuiManager.h"
#include "Common/Logs.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"

#include "Common/Descriptors/SRV.h"

#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace KashipanEngine {

ImGuiManager::ImGuiManager(WinApp *winApp, DirectXCommon *dxCommon) {
    // nullチェック
    if (winApp == nullptr) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }
    // 引数をメンバ変数に格納
    winApp_ = winApp;
    dxCommon_ = dxCommon;

    // srvDescriptorHeapの初期化
    SRV::Initialize(dxCommon_);

    // ImGuiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp_->GetWindowHandle());
    ImGui_ImplDX12_Init(
        dxCommon_->GetDevice(),
        dxCommon_->GetSwapChainDesc().BufferCount,
        dxCommon_->GetRTVDesc().Format,
        SRV::GetDescriptorHeap(),
        SRV::GetCPUDescriptorHandle(),
        SRV::GetGPUDescriptorHandle()
    );

    // 日本語フォントを設定
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    // 初期化完了のログを出力
    Log("ImGuiManager Initialized.");
    LogNewLine();
}

ImGuiManager::~ImGuiManager() {
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // 終了処理完了のログを出力
    Log("ImGuiManager Finalized.");
    LogNewLine();
}

void ImGuiManager::Begin(const char *name) {
    ImGui::Begin(name);
    // ImGui上でのフォントサイズを設定
    auto dpi = GetDpiForSystem();
    ImGui::SetWindowFontScale(static_cast<float>(dpi) / 96.0f);
}

void ImGuiManager::BeginFrame() {
    // ImGuiのフレーム開始処理
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
    // ImGuiのフレーム終了処理
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
}

void ImGuiManager::Reinitialize() {
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // ImGuiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp_->GetWindowHandle());
    ImGui_ImplDX12_Init(
        dxCommon_->GetDevice(),
        dxCommon_->GetSwapChainDesc().BufferCount,
        dxCommon_->GetRTVDesc().Format,
        SRV::GetDescriptorHeap(),
        SRV::GetCPUDescriptorHandle(),
        SRV::GetGPUDescriptorHandle()
    );
}

} // namespace KashipanEngine