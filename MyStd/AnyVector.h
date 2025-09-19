#pragma once
#include <vector>
#include <any>
#include <stdexcept>

namespace MyStd {

class AnyVector {
public:
    AnyVector() = default;
    ~AnyVector() = default;

    template<typename T>
    operator T() const {
        return GetTo<T>();
    }

    AnyVector &operator[](size_t index) {
        if (index >= variables_.size()) {
            // 指定されたインデックスが現在のサイズを超えている場合はサイズを拡張
            variables_.resize(index + 1);
        }
        return variables_[index];
    }
    AnyVector &operator=(const std::any &value) {
        value_ = value;
        return *this;
    }

    auto begin() const {
        return variables_.begin();
    }
    auto end() const {
        return variables_.end();
    }
    size_t size() const {
        return variables_.size();
    }
    AnyVector &at(size_t index) {
        if (index >= variables_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return variables_[index];
    }
    void push_back(const AnyVector &value) {
        variables_.push_back(value);
    }
    void push_back(const std::any &value) {
        variables_.emplace_back().value_ = value;
    }
    void pop_back() {
        if (!variables_.empty()) {
            variables_.pop_back();
        }
    }
    AnyVector &front() {
        if (variables_.empty()) {
            throw std::out_of_range("Vector is empty");
        }
        return variables_.front();
    }
    AnyVector &back() {
        if (variables_.empty()) {
            throw std::out_of_range("Vector is empty");
        }
        return variables_.back();
    }
    bool empty() const {
        return variables_.empty();
    }
    void clear() {
        variables_.clear();
    }

    const char *GetTypeName() const {
        return value_.type().name();
    }
    template<typename T>
    T GetTo() const {
        return std::any_cast<T>(value_);
    }
    template<typename T>
    T GetTo(const std::string &typeName) const {
        if (GetTypeName() == typeName) {
            return std::any_cast<T>(value_);
        } else {
            throw std::bad_any_cast();
        }
    }

private:
    std::vector<AnyVector> variables_;
    std::any value_;
};

} // namespace MyStd