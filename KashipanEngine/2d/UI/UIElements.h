#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Math/Matrix3x3.h"

namespace KashipanEngine {

class UIElements {
public:
    UIElements() {
        elements_["pos"]            = std::make_unique<TypedElement<Vector2>>(Vector2(0.0f, 0.0f));
        elements_["scale"]          = std::make_unique<TypedElement<Vector2>>(Vector2(1.0f, 1.0f));
        elements_["anchor"]         = std::make_unique<TypedElement<Vector2>>(Vector2(0.0f, 0.0f));
        elements_["pivot"]          = std::make_unique<TypedElement<Vector2>>(Vector2(0.0f, 0.0f));
        elements_["offset"]         = std::make_unique<TypedElement<Vector2>>(Vector2(0.0f, 0.0f));
        elements_["color"]          = std::make_unique<TypedElement<Vector4>>(Vector4(255.0f, 255.0f, 255.0f, 255.0f));
        elements_["textureIndex"]   = std::make_unique<TypedElement<int>>(-1);
        elements_["degree"]         = std::make_unique<TypedElement<float>>(0.0f);
        elements_["isVisible"]      = std::make_unique<TypedElement<bool>>(true);
    }

    /// @brief 要素の取得
    /// @tparam T 型
    /// @param element 要素名
    /// @return 要素の値
    template <typename T>
    const T &Get(const std::string &element) {
        // 要素が存在しない場合は新しく作成して返す
        if (elements_.find(element) == elements_.end()) {
            elements_[element] = std::make_unique<TypedElement<T>>(T{});
        }
        return dynamic_cast<TypedElement<T> *>(elements_[element].get())->value;
    }

    /// @brief 要素の設定
    /// @tparam T 型
    /// @param element 要素名
    /// @param value 設定する値
    template <typename T>
    void Set(const std::string &element, const T &value) {
        if (elements_.find(element) == elements_.end()) {
            elements_[element] = std::make_unique<TypedElement<T>>(value);
        }
        dynamic_cast<TypedElement<T> *>(elements_[element].get())->value = value;
    }

private:
    // 要素テーブル作成用クラス
    class Element {
    public:
        virtual ~Element() = default;
    };
    template <typename T>
    class TypedElement : public Element {
    public:
        T value;
        explicit TypedElement(const T &val) : value(val) {}
    };

    std::unordered_map<std::string, std::unique_ptr<Element>> elements_;
};

}