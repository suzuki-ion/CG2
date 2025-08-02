#include <numbers>
#include "BaseUI.h"

namespace KashipanEngine {

BaseUI::BaseUI(const std::string &name, Renderer *renderer) : name_(name) {
    sprite_ = std::make_unique<Sprite>("Resources/white1x1.png");
    sprite_->SetRenderer(renderer);
    worldTransform_ = std::make_unique<WorldTransform>();
}

void BaseUI::AddChild(BaseUI *child) {
    if (children_.find(child->GetName()) == children_.end()) {
        children_[child->GetName()] = child;
    }
}

void BaseUI::RemoveChild(const std::string &name) {
    auto it = children_.find(name);
    if (it != children_.end()) {
        children_.erase(it);
    }
}

BaseUI *BaseUI::GetChild(const std::string &name) const {
    auto it = children_.find(name);
    if (it != children_.end()) {
        return it->second;
    }
    return nullptr;
}

void BaseUI::UpdateCommon() {
    worldTransform_->translate_ = Vector3(uiElements_.Get<Vector2>("pos"));
    worldTransform_->scale_ = Vector3(uiElements_.Get<Vector2>("scale"));
    float radian = uiElements_.Get<float>("degree") * (std::numbers::pi_v<float> / 180.0f);
    worldTransform_->rotate_ = Vector3(0.0f, 0.0f, radian);

    auto sprite = sprite_->GetStatePtr();
    sprite.material->color = uiElements_.Get<Vector4>("color");

    worldTransform_->TransferMatrix();
    for (auto &child : children_) {
        child.second->SetParentWorldMatrix(worldTransform_.get());
        child.second->UpdateCommon();
    }
    UpdateEvent();
    ResetEventFrags();
}

void BaseUI::UpdateEvent() {
    for (auto &eventCheck : eventChecks_) {
        if (eventCheck.second(this)) {
            size_t index = static_cast<size_t>(eventCheck.first);
            eventFrags_.flags[index] = true;
        }
    }
    for (auto &event : eventFunctions_) {
        event.second();
    }
    for (auto &child : children_) {
        child.second->UpdateEvent();
    }
}

} // namespace KashipanEngine