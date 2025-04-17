#pragma once
#include <d3d12.h>
#include <imgui.h>
#include <wrl.h>

namespace MyEngine {

class ImGuiManager final {
public:
    ImGuiManager(const ImGuiManager &) = delete;
    ImGuiManager &operator=(const ImGuiManager &) = delete;
    ImGuiManager(ImGuiManager &&) = delete;
    ImGuiManager &operator=(ImGuiManager &&) = delete;

    /// @brief インスタンス取得
    /// @return ImGuiManagerクラスのインスタンス
    static ImGuiManager *GetInstance() {
        static ImGuiManager instance;
        return &instance;
    }

    /// @brief ImGui初期化
    void Initialize();

    /// @brief ImGui終了処理
    void Finalize();

private:
    ImGuiManager() = default;
    ~ImGuiManager() = default;
};

} // namespace MyEngine