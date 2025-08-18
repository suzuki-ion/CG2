#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>

namespace MyStd {

class AnyUnorderedMap {
public:
    AnyUnorderedMap() = default;
    ~AnyUnorderedMap() = default;

    template<typename T>
    operator T() const {
        return GetTo<T>();
    }

    AnyUnorderedMap &operator[](const std::string &key) {
        return variables_[key];
    }
    AnyUnorderedMap &operator[](const char *key) {
        return variables_[key];
    }
    AnyUnorderedMap &operator=(const std::any &value) {
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
    AnyUnorderedMap &at(const std::string &key) {
        auto it = variables_.find(key);
        if (it == variables_.end()) {
            throw std::out_of_range("Key not found: " + key);
        }
        return it->second;
    }
    AnyUnorderedMap &at(const char *key) {
        return at(std::string(key));
    }
    void clear() {
        variables_.clear();
    }
    bool empty() const {
        return variables_.empty();
    }
    void erase(const std::string &key) {
        auto it = variables_.find(key);
        if (it != variables_.end()) {
            variables_.erase(it);
        }
    }
    void erase(const char *key) {
        erase(std::string(key));
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
    std::unordered_map<std::string, AnyUnorderedMap> variables_;
    std::any value_;
};

} // namespace MyStd