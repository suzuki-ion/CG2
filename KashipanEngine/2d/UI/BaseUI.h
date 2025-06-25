#pragma once
#include <string>
#include <vector>
#include "Math/Vector2.h"
#include "Math/Vector4.h"

namespace KashipanEngine {

class BaseUI {
public:
    struct UIData {
        Vector2 pos;
        Vector2 size;
        Vector2 anchor;
        Vector2 pivot;
        Vector2 offset;
        Vector4 color;
        float degree;
        bool visible;
    };

    BaseUI(const std::string &name) : name(name) {
    }
    virtual ~BaseUI() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() const = 0;

    std::string_view GetName() const { return name; }
    
private:
    const std::string name_;
    Vector2 pos_ = { 0.0f, 0.0f };
    Vector2 size_ = { 0.0f, 0.0f };
    Vector2 anchor_ = { 0.0f, 0.0f };
    Vector2 pivot_ = { 0.5f, 0.5f };
    Vector2 offset_ = { 0.0f, 0.0f };
    Vector4 color_ = { 255.0f, 255.0f, 255.0f, 255.0f };
    float degree_ = 0.0f;
    bool visible_ = true;

    std::vector<BaseUI *> children_;
};

} // namespace KashipanEngine