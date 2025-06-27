#pragma once
#include "IsClick.h"

namespace KashipanEngine {

enum class UIComponents {
    kIsClick,
    kIsHover,
    kIsFocus,
    kIsActive,
    kIsDrag,
    kIsScroll,
    kIsAnimation
};

class UIComponent {
public:

    void Update();

private:
    IsClick *isClick = nullptr;
};

} // namespace KashipanEngine