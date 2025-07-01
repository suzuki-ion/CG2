#pragma once
#include <Objects/Sprite.h>
#include <bitset>
#include <functional>
#include "UIElements.h"

namespace KashipanEngine {

struct UIEventFrags {
    enum class EventType {
        kClick,     // クリックされたとき
        kClicking,  // クリックしている間
        kClicked,   // クリックが終了したとき
        kSelect,    // 選択されたとき
        kHover,     // カーソルがUIの上にあるとき
        kFocus,     // 入力欄がアクティブになったとき
        kGrab,      // 要素を掴んでいるとき
        kDrag,      // 要素を掴んで動かしているとき
        kResize,    // UIのサイズが変更されたとき
        kActive,    // UIがアクティブな状態のとき

        // イベントの総数
        kCount
    };
    std::bitset<static_cast<size_t>(EventType::kCount)> flags;
};

class BaseUI {
public:
    BaseUI() = delete;
    BaseUI(const std::string &name, Renderer *renderer);
    virtual ~BaseUI() = default;

    virtual void Update() = 0;
    virtual void Draw() const = 0;

    /// @brief イベント通知関数
    /// @param eventType イベントの種類
    void OnEvent(UIEventFrags::EventType eventType) {
        eventFrags_.flags.set(static_cast<size_t>(eventType), true);
    }

    /// @brief イベントが発生しているかどうかの確認用関数
    /// @param eventType イベントの種類
    /// @return true: イベントが発生している, false: 発生していない
    bool IsEvented(UIEventFrags::EventType eventType) const {
        return eventFrags_.flags.test(static_cast<size_t>(eventType));
    }

    void AddChild(BaseUI *child);
    void RemoveChild(const std::string &name);
    BaseUI *GetChild(const std::string &name) const;

    const std::string &GetName() const {
        return name_;
    }
    template <typename T>
    const T &GetUIElement(const std::string &name) {
        return uiElements_.Get<T>(name);
    }
    const UIEventFrags &GetEventFrags() const {
        return eventFrags_;
    }
    const WorldTransform &GetWorldTransform() {
        return *worldTransform_;
    }

    template <typename T>
    void SetUIElement(const std::string &name, const T &val) {
        if constexpr (std::is_convertible_v<T, uint32_t>) {
            if (name == "textureIndex") {
                sprite_->SetTexture(static_cast<uint32_t>(val));
            }
        }
        uiElements_.Set(name, val);
    }

    void SetParentWorldMatrix(const WorldTransform *parentWorldTransform) {
        worldTransform_->parentTransform_ = parentWorldTransform;
    }
    
protected:
    void UpdateCommon();
    void UpdateEvent();

    /// @brief イベントリセット用関数
    void ResetEventFrags() {
        eventFrags_.flags.reset();
    }

    /// @brief UIの名前(識別用)
    const std::string name_;

    std::unique_ptr<Sprite> sprite_ = nullptr;
    std::unique_ptr<WorldTransform> worldTransform_ = nullptr;
    UIElements uiElements_;
    UIEventFrags eventFrags_ = {};

    std::unordered_map<UIEventFrags::EventType, std::function<bool(BaseUI *)>> eventChecks_;
    std::unordered_map<UIEventFrags::EventType, std::function<void()>> eventFunctions_;
    std::unordered_map<std::string, BaseUI *> children_;
};

} // namespace KashipanEngine