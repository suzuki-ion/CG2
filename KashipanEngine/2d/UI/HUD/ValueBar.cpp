#include "ValueBar.h"
#include "2d/UI/Component/InputDetector.h"

namespace KashipanEngine {

ValueBar::ValueBar(const std::string &name, Renderer *renderer) : BaseUI(name, renderer) {
}

void ValueBar::Update() {
    UpdateCommon();
}

void ValueBar::Draw() const {
    sprite_->Draw(*worldTransform_);
    for (auto &child : children_) {
        child.second->Draw();
    }
}

} // namespace KashipanEngine