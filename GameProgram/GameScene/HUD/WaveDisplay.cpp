#include "WaveDisplay.h"
#include <format>

using namespace KashipanEngine;

WaveDisplay::WaveDisplay() {
    waveText_ = std::make_unique<Text>(16);
    waveText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    waveText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    waveText_->SetTextAlign(TextAlignX::Right, TextAlignY::Top);

    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_ = Vector3(1920.0f - 16.0f, 16.0f, 0.0f);
}

void WaveDisplay::SetWave(int current, int max) {
    if (current > max) {
        current = max;
    }
    std::string waveStr = std::format("{}/{} WAVE", current, max);
    waveText_->SetText(std::u8string(waveStr.begin(), waveStr.end()));
}

void WaveDisplay::Draw() const {
    waveText_->SetPipelineName("Object3d.Solid.BlendNormal");
    waveText_->Draw(*worldTransform_);
    waveText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    waveText_->Draw(*worldTransform_);
}