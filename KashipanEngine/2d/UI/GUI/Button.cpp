#include "Button.h"
#include "../Component/InputDetector.h"

namespace KashipanEngine {

Button::Button(const std::string &name, Renderer *renderer) : BaseUI(name, renderer) {
    eventChecks_[UIEventFrags::EventType::kClicking] = UIInputDetector::IsClicking;
    eventChecks_[UIEventFrags::EventType::kHover] = UIInputDetector::IsHover;
    eventFunctions_[UIEventFrags::EventType::kClicking] = [this]() { ClickingEvent(); };
    eventFunctions_[UIEventFrags::EventType::kHover] = [this]() { HoverEvent(); };
}

void Button::Update() {
    UpdateCommon();
}

void Button::Draw() const {
    sprite_->Draw(*worldTransform_);
    for (auto &child : children_) {
        child.second->Draw();
    }
}

void Button::ClickingEvent() {
    const size_t kClicking = static_cast<size_t>(UIEventFrags::EventType::kClicking);
    auto color = uiElements_.Get<Vector4>("color");

    if (eventFrags_.flags.test(kClicking)) {
        color = { 255.0f, 0.0f, 0.0f, 255.0f };
    } else {
        color = { 255.0f, 255.0f, 255.0f, 255.0f };
    }

    uiElements_.Set("color", color);
}

void Button::HoverEvent() {
    const size_t kClicking = static_cast<size_t>(UIEventFrags::EventType::kClicking);
    if (eventFrags_.flags.test(kClicking)) {
        return;
    }

    const size_t kHover = static_cast<size_t>(UIEventFrags::EventType::kHover);
    auto color = uiElements_.Get<Vector4>("color");

    if (eventFrags_.flags.test(kHover)) {
        color = { 255.0f, 196.0f, 196.0f, 255.0f };
    } else {
        color = { 255.0f, 255.0f, 255.0f, 255.0f };
    }
    
    uiElements_.Set("color", color);
}

} // namespace KashipanEngine