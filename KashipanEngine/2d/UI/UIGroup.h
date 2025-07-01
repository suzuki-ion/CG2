#pragma once
#include <vector>
#include "BaseUI.h"

namespace KashipanEngine {

class UIGroup : public BaseUI {
public:
    UIGroup() = delete;
    UIGroup(const std::string& name, Renderer *renderer)
        : BaseUI(name, renderer) {}
    void Update() override;
    void Draw() const override;
};

} // namespace KashipanEngine