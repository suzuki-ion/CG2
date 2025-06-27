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

    const std::string &GetName() const {
        return name_;
    }
    const Vector2 &GetPosition() const {
        return pos_;
    }
    const Vector2 &GetSize() const {
        return size_;
    }
    const Vector2 &GetAnchor() const {
        return anchor_;
    }
    const Vector2 &GetPivot() const {
        return pivot_;
    }
    const Vector2 &GetOffset() const {
        return offset_;
    }
    const Vector4 &GetColor() const {
        return color_;
    }
    float GetDegree() const {
        return degree_;
    }
    bool IsVisible() const {
        return visible_;
    }
    const Matrix3x3 &GetLocalMatrix() const {
        return localMatrix_;
    }
    const Matrix3x3 &GetWorldMatrix() const {
        return worldMatrix_;
    }

    void SetPosition(const Vector2 &pos) {
        pos_ = pos;
    }
    void SetSize(const Vector2 &size) {
        size_ = size;
    }
    void SetAnchor(const Vector2 &anchor) {
        anchor_ = anchor;
    }
    void SetPivot(const Vector2 &pivot) {
        pivot_ = pivot;
    }
    void SetOffset(const Vector2 &offset) {
        offset_ = offset;
    }
    void SetColor(const Vector4 &color) {
        color_ = color;
    }
    void SetDegree(float degree) {
        degree_ = degree;
    }
    void SetVisible(bool visible) {
        visible_ = visible;
    }
    void SetParentMatrix(const Matrix3x3 &parentMatrix) {
        parentMatrix_ = parentMatrix;
    }
    
private:
    const std::string name_;
    Vector2 pos_ = { 0.0f, 0.0f };
    Vector2 size_ = { 0.0f, 0.0f };
    Vector2 anchor_ = { 0.0f, 0.0f };
    Vector2 pivot_ = { 0.5f, 0.5f };
    Vector2 offset_ = { 0.0f, 0.0f };
    Vector4 color_ = { 255.0f, 255.0f, 255.0f, 255.0f };
    Matrix3x3 localMatrix_ = Matrix3x3::Identity();
    Matrix3x3 worldMatrix_ = Matrix3x3::Identity();
    Matrix3x3 parentMatrix_ = Matrix3x3::Identity();
    float degree_ = 0.0f;
    bool visible_ = true;

    std::unordered_map<std::string, BaseUI *> children_;
};

} // namespace KashipanEngine