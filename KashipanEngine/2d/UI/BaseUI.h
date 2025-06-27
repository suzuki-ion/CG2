#pragma once
#include <string>
#include <unordered_map>
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Math/Matrix3x3.h"

namespace KashipanEngine {

class BaseUI {
public:
    BaseUI(const std::string &name) : name_(name) {
    }
    virtual ~BaseUI() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() const = 0;
    
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

    std::unordered_map<std::string, BaseUI *> children_;
};

} // namespace KashipanEngine