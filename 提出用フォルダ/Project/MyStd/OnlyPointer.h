#pragma once

namespace mystd {

template <typename T>
class OnlyPointer {
public:
    OnlyPointer() = default;
    OnlyPointer(T *ptr) { ptr_ = ptr; }

    void operator=(T *ptr) {
        if (ptr == nullptr) {
            Reset();
            return;
        }
        if (ptr_ != nullptr) {
            return;
        }
        ptr_ = ptr;
    }
    T &operator*() { return *ptr_; }
    T *operator->() { return ptr_; }
    T *Get() { return ptr_; }
    void Reset() { ptr_ = nullptr; }
    bool IsNull() { return ptr_ == nullptr; }

private:
    T *ptr_ = nullptr;
};

}