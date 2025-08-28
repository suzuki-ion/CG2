#pragma once
#include "../BaseUI.h"

namespace KashipanEngine {

class ValueBar : public BaseUI {
public:
    ValueBar() = delete;
    ValueBar(const std::string &name, Renderer *renderer);

    void Update() override;
    void Draw() const override;

    void SetValueRange(float min, float max) {
        valueMin_ = min;
        valueMax_ = max;
        if (valueCurrent_ < valueMin_) valueCurrent_ = valueMin_;
        if (valueCurrent_ > valueMax_) valueCurrent_ = valueMax_;
    }
    void SetCurrentValue(float value) {
        if (value < valueMin_) valueCurrent_ = valueMin_;
        else if (value > valueMax_) valueCurrent_ = valueMax_;
        else valueCurrent_ = value;
    }

protected:
    float valueMin_ = 0.0f;
    float valueMax_ = 100.0f;
    float valueCurrent_ = 50.0f;
};

} // namespace KashipanEngine