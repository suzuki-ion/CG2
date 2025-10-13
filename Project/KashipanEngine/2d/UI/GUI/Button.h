#pragma once
#include "2d/UI/BaseUI.h"

namespace KashipanEngine {

class Button : public BaseUI {
public:
    Button() = delete;
    Button(const std::string &name, Renderer *renderer);

    void Update() override;
    void Draw() const override;

protected:
    void ClickingEvent();
    void HoverEvent();
};

} // namespace KashipanEngine