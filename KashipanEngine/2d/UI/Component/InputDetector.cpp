#include "InputDetector.h"
#include "Base/Input.h"
#include "Base/Texture.h"

namespace KashipanEngine::UIInputDetector {

bool IsClick(BaseUI *detectUI) {
    bool isHover = IsHover(detectUI);
    if (!isHover) return false;

    bool isLeftClick = Input::IsMouseButtonTrigger(0);
    return isLeftClick;
}

bool IsClicking(BaseUI *detectUI) {
    bool isHover = IsHover(detectUI);
    if (!isHover) return false;

    bool isLeftClicking = Input::IsMouseButtonDown(0);
    return isLeftClicking;
}

bool IsClicked(BaseUI *detectUI) {
    bool isHover = IsHover(detectUI);
    if (!isHover) return false;

    bool isLeftClicked = Input::IsMouseButtonRelease(0);
    return isLeftClicked;
}

bool IsSelect(BaseUI *detectUI) {
    return false;
}

bool IsHover(BaseUI *detectUI) {
    Vector2 mousePos(
        static_cast<float>(Input::GetMouseX()),
        static_cast<float>(Input::GetMouseY())
    );
    Vector2 uiPos(
        detectUI->GetWorldTransform().worldMatrix_.m[3][0],
        detectUI->GetWorldTransform().worldMatrix_.m[3][1]
    );
    uint32_t textureIndex = detectUI->GetUIElement<int>("textureIndex");
    TextureData textureData = Texture::GetTexture(textureIndex);
    Vector2 uiSize(
        static_cast<float>(textureData.width),
        static_cast<float>(textureData.height)
    );

    return (mousePos.x >= uiPos.x && mousePos.x <= uiPos.x + uiSize.x) &&
           (mousePos.y >= uiPos.y && mousePos.y <= uiPos.y + uiSize.y);
}

bool IsFocus(BaseUI *detectUI) {
    return false;
}

bool IsGrab(BaseUI *detectUI) {
    return false;
}

bool IsDrag(BaseUI *detectUI) {
    return false;
}

bool IsResize(BaseUI *detectUI) {
    return false;
}

} // namespace KashipanEngine::UIInputDetector