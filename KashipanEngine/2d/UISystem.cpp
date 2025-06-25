#include <unordered_map>
#include "UISystem.h"

namespace KashipanEngine {

namespace {
// UI情報を保持するためのマップ
std::unordered_map<std::string, BaseUI *> uiMap;
} // namespace

void UISystem::AddUI(BaseUI *ui) {
    if (ui == nullptr) {
        return; // nullptrチェック
    }
    std::string name = ui->
    if (uiMap.find(name) != uiMap.end()) {
        // 既に同じ名前のUIが存在する場合は上書き
        delete uiMap[name]; // 古いUIを削除
    }
    uiMap[name] = ui; // 新しいUIを追加
}

void UISystem::RemoveUI(std::string name) {

}


} // namespace KashipanEngine