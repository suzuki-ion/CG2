// ScreenBufferのマウス座標テスト用コード
// このコードをmain.cppや適切な場所に追加してテストしてください

#ifdef _DEBUG
void TestScreenBufferMouseCoordinates(ScreenBuffer* screenBuffer) {
    if (!screenBuffer) return;

    // マウス座標を取得
    int mouseX = Input::GetMouseX();
    int mouseY = Input::GetMouseY();
    
    // スクリーンの情報を取得
    Vector2 scale = screenBuffer->GetCurrentScale();
    Vector2 leftTop = screenBuffer->GetCurrentLeftTopPos();
    
    // デバッグ情報を取得
    Vector2 imageSize, windowPos, contentMin;
    screenBuffer->GetDebugInfo(imageSize, windowPos, contentMin);
    
    // ImGuiでデバッグ情報を表示
    ImGui::Begin("ScreenBuffer Debug Info");
    
    ImGui::Text("Mouse Position (Screen): (%d, %d)", 
        Input::GetMousePos(Input::CurrentOption::Current, Input::AxisOption::X, Input::ValueOption::Actual),
        Input::GetMousePos(Input::CurrentOption::Current, Input::AxisOption::Y, Input::ValueOption::Actual));
    
    ImGui::Text("Mouse Position (Buffer): (%d, %d)", mouseX, mouseY);
    
    ImGui::Text("Screen Scale: (%.3f, %.3f)", scale.x, scale.y);
    ImGui::Text("Screen Left-Top: (%.1f, %.1f)", leftTop.x, leftTop.y);
    ImGui::Text("Image Size: (%.1f, %.1f)", imageSize.x, imageSize.y);
    
    // 座標変換テスト
    int bufferX, bufferY;
    int screenMouseX = Input::GetMousePos(Input::CurrentOption::Current, Input::AxisOption::X, Input::ValueOption::Actual);
    int screenMouseY = Input::GetMousePos(Input::CurrentOption::Current, Input::AxisOption::Y, Input::ValueOption::Actual);
    
    bool isInside = screenBuffer->ScreenToBuffer(screenMouseX, screenMouseY, bufferX, bufferY);
    
    ImGui::Text("Screen to Buffer conversion:");
    ImGui::Text("  Screen: (%d, %d)", screenMouseX, screenMouseY);
    ImGui::Text("  Buffer: (%d, %d)", bufferX, bufferY);
    ImGui::Text("  Inside: %s", isInside ? "Yes" : "No");
    
    if (isInside) {
        ImGui::Text("Coordinate check: PASS");
    } else {
        ImGui::Text("Coordinate check: Mouse outside screen area");
    }
    
    ImGui::End();
}
#endif

// 使用例:
// ゲームループ内で以下のように呼び出します：
// 
// #ifdef _DEBUG
// TestScreenBufferMouseCoordinates(yourScreenBuffer);
// #endif