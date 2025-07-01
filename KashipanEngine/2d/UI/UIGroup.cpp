#include "UIGroup.h"

namespace KashipanEngine {

void UIGroup::Update() {
    UpdateCommon();
}

void UIGroup::Draw() const {
    sprite_->Draw(*worldTransform_);
    for (auto &child : children_) {
        child.second->Draw();
    }
}

} // namespace KashipanEngine