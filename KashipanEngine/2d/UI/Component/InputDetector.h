#pragma once
#include "../BaseUI.h"

namespace KashipanEngine::UIInputDetector {

bool IsClick(BaseUI *detectUI);
bool IsClicking(BaseUI *detectUI);
bool IsClicked(BaseUI *detectUI);
bool IsSelect(BaseUI *detectUI);
bool IsHover(BaseUI *detectUI);
bool IsFocus(BaseUI *detectUI);
bool IsGrab(BaseUI *detectUI);
bool IsDrag(BaseUI *detectUI);
bool IsResize(BaseUI *detectUI);

} // namespace KashipanEngine