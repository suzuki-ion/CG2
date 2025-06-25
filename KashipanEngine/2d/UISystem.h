#pragma once
#include "UI/BaseUI.h"

namespace KashipanEngine {

class UISystem {
public:
    /// @brief 初期化用関数
    static void Initialize();
    /// @brief 終了用関数
    static void Finalize();

    /// @brief 更新用関数
    static void Update();
    /// @brief 描画用関数
    static void Draw();

    /// @brief UI追加関数
    /// @param ui 追加するUIへのポインタ
    static void AddUI(BaseUI *ui);

    /// @brief UI削除関数
    /// @param name 削除するUIの名前
    static void RemoveUI(std::string name);

    /// @brief UI取得関数
    /// @param name 取得するUIの名前
    /// @return UIへのポインタ
    static BaseUI *GetUI(std::string name);

    /// @brief 全てのUIを取得する関数
    /// @return 全てのUIへのポインタのベクターの参照
    static std::vector<BaseUI *> &GetAllUIs();
};

} // namespace KashipanEngine