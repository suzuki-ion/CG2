#pragma once

namespace KashipanEngine {

class BaseUI;

class IsClick {
public:
    IsClick() = delete;
    IsClick(BaseUI *parentUI) :
        parentUI_(parentUI) {}

private:
    const BaseUI *const parentUI_;
};

} // namespace KashipanEngine