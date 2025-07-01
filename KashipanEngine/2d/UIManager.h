#pragma once
#include "UI/BaseUI.h"

namespace KashipanEngine {

class Renderer;

class UIManager {
public:
    /// @brief レンダラー設定関数
    /// @param renderer 設定するレンダラーへのポインタ
    void SetRenderer(Renderer *renderer);

    /// @brief 更新用関数
    void Update();
    /// @brief 描画用関数
    void Draw();

    /// @brief UI追加関数
    /// @param ui 追加するUIへのポインタ
    void AddUI(BaseUI *ui);
    /// @brief UI削除関数
    /// @param name 削除するUIの名前
    void RemoveUI(std::string name);

    /// @brief UI作関数
    /// @param name UIの名前
    /// @return 作成されたUIへのポインタ
    BaseUI *CreateUI(std::string name);

    /// @brief UI取得関数
    /// @param name 取得するUIの名前
    /// @return UIへのポインタ
    BaseUI *GetUI(std::string name);

    /// @brief 全てのUIを取得する関数
    /// @return 全てのUIへのポインタのベクターの参照
    std::vector<BaseUI *> &GetAllUIs();
};

} // namespace KashipanEngine