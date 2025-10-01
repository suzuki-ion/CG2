#pragma once
#include <Objects/Text.h>

class WaveDisplay {
public:
    WaveDisplay();
    ~WaveDisplay() = default;

    void SetWave(int current, int max);
    void Draw() const;

private:
    int waveCurrent_ = 0;
    int waveMax_ = 0;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Text> waveText_;
};