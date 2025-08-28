#pragma once
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace MyStd {

template<typename Key, typename Value>
struct Entry {
    Entry() = delete;
    Entry(const Key &setKey, const size_t &setIndex) :
        key(setKey), index(setIndex), value(Value{}) {
    }
    Entry(const Key &setKey, const size_t &setIndex, const Value &setValue) :
        key(setKey), index(setIndex), value(setValue) {
    }

    operator Value &() {
        return value;
    }
    operator const Value &() const {
        return value;
    }

    Entry &operator=(const Value &newValue) {
        value = newValue;
        return *this;
    }
    Entry &operator+=(const Value &newValue) {
        value += newValue;
        return *this;
    }
    Entry &operator-=(const Value &newValue) {
        value -= newValue;
        return *this;
    }
    Entry &operator*=(const Value &newValue) {
        value *= newValue;
        return *this;
    }
    Entry &operator/=(const Value &newValue) {
        if (newValue == 0) {
            // ゼロ除算対策に0を代入
            value = 0;
        } else {
            value /= newValue;
        }
        return *this;
    }
    Entry &operator+(const Value &newValue) {
        value += newValue;
        return *this;
    }
    Entry &operator-(const Value &newValue) {
        value -= newValue;
        return *this;
    }
    Entry &operator*(const Value &newValue) {
        value *= newValue;
        return *this;
    }
    Entry &operator/(const Value &newValue) {
        if (newValue == 0) {
            // ゼロ除算対策に0を代入
            value = 0;
        } else {
            value /= newValue;
        }
        return *this;
    }
    Entry &operator++() {
        ++value;
        return *this;
    }
    Entry &operator--() {
        --value;
        return *this;
    }

    const Key key;
    const size_t index;
    Value value;
};

template<typename Key, typename Value>
class VectorMap {
public:
    VectorMap() = default;
    ~VectorMap() = default;

    Entry<Key, Value> &operator[](const Key &key) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            return vec_[it->second];
        } else {
            // キーが存在しない場合は新しい値を追加
            vec_.emplace_back(key, vec_.size());
            // 新しい要素のインデックスをマップに保存
            map_[key] = vec_.size() - 1;
            return vec_.back();
        }
    }
    Entry<Key, Value> &operator[](size_t index) {
        return at(index);
    }

    void push_back(const Key &key, const Value &value) {
        // キーが存在しない場合は新しい値を追加
        if (map_.find(key) == map_.end()) {
            vec_.emplace_back(key, vec_.size(), value);
            map_[key] = vec_.size() - 1;
        } else {
            // 既に存在するキーの場合は値を更新
            vec_[map_[key]].value = value;
        }
    }
    void push_back(const std::pair<Key, Value> &pair) {
        push_back(pair.first, pair.second);
    }
    void pop_back() {
        if (!vec_.empty()) {
            Key key = vec_.back().key;
            // マップから削除
            map_.erase(key);
            // ベクターから削除
            vec_.pop_back();
        }
    }
    Entry<Key, Value> &front() {
        if (vec_.empty()) {
            throw std::out_of_range("Vector is empty");
        }
        return vec_.front();
    }
    Entry<Key, Value> &back() {
        if (vec_.empty()) {
            throw std::out_of_range("Vector is empty");
        }
        return vec_.back();
    }

    size_t size() const {
        return vec_.size();
    }
    bool empty() const {
        return vec_.empty();
    }
    void clear() {
        vec_.clear();
        map_.clear();
    }
    void erase(const Key &key) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            // マップから削除
            map_.erase(it);
            // ベクターから削除
            vec_.erase(std::remove_if(vec_.begin(), vec_.end(),
                [&key](const std::pair<Key, Value> &p) { return p.first == key; }), vec_.end());
        }
    }
    void erase(size_t index) {
        if (index < vec_.size()) {
            Key key = vec_[index].key;
            // マップから削除
            map_.erase(key);
            // ベクターから削除
            vec_.erase(vec_.begin() + index);
        }
    }
    Entry<Key, Value> &at(const Key &key) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            throw std::out_of_range("Key not found");
        }
        return vec_[it->second];
    }
    Entry<Key, Value> &at(size_t index) {
        if (index >= vec_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return vec_[index];
    }
    auto find(const Key &key) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return vec_.end();
        }
        return vec_.begin() + it->second;
    }
    auto find(size_t index) {
        if (index >= vec_.size()) {
            return vec_.end();
        }
        return vec_.begin() + index;
    }

    auto begin() {
        return vec_.begin();
    }
    auto end() {
        return vec_.end();
    }

private:
    // ベクターは値の実体を保持
    std::vector<Entry<Key, Value>> vec_;
    // マップは値へのインデックスを保持
    std::unordered_map<Key, size_t> map_;
};

} // namespace MyStd