#include <algorithm>
#include <stdexcept>

#include "KashipanEngine.h"
#include "KeyFrameAnimation.h"

namespace KashipanEngine {

namespace {

bool TimeCompare(const KeyFrame &a, const KeyFrame &b) {
    return a.timeSec < b.timeSec;
}

} // namespace

void KeyFrameAnimation::Update() {
    if (!isPlaying_ ||
        duration_ <= 0.0f ||
        keyFrameElements_.empty()) {
        return;
    }

    currentTime_ += Engine::GetDeltaTime() * playSpeed_;
    if (currentTime_ >= duration_) {
        if (isLoop_) {
            currentTime_ = fmod(currentTime_, duration_);
        } else {
            isPlaying_ = false;
            currentTime_ = duration_;
        }
    }

    for (auto &pair : keyFrameElements_) {
        UpdateKeyFrameElementData(pair.second);
    }
}

void KeyFrameAnimation::AddKeyFrame(KeyFrameElementType type, const KeyFrame &keyFrame) {
    auto &element = keyFrameElements_[type];
    element.keyFrames.push_back(keyFrame);

    // 順番が狂わないようにする
    SortKeyFrames(element);
    // アニメーションの長さを更新
    UpdateDuration();
}

void KeyFrameAnimation::RemoveKeyFrame(KeyFrameElementType type, size_t index) {
    if (keyFrameElements_.find(type) == keyFrameElements_.end()) {
        throw std::invalid_argument("Invalid key frame element type.");
    }
    auto &element = keyFrameElements_[type];
    if (index < element.keyFrames.size()) {
        element.keyFrames.erase(element.keyFrames.begin() + index);
    } else {
        throw std::out_of_range("Index out of range for key frames.");
    }
    // アニメーションの長さを更新
    UpdateDuration();
}

void KeyFrameAnimation::SetKeyFrameElementData(KeyFrameElementType type, const KeyFrameElementData &data) {
    keyFrameElements_[type] = data;
    SortKeyFrames(keyFrameElements_[type]);
    UpdateKeyFrameElementData(keyFrameElements_[type]);
    // アニメーションの長さを更新
    UpdateDuration();
}

void KeyFrameAnimation::SetKeyFrame(KeyFrameElementType type, size_t index, const KeyFrame &keyFrame) {
    if (index < keyFrameElements_[type].keyFrames.size()) {
        keyFrameElements_[type].keyFrames[index] = keyFrame;
    } else {
        keyFrameElements_[type].keyFrames.push_back(keyFrame);
    }
    // 順番が狂わないようにする
    SortKeyFrames(keyFrameElements_[type]);
    UpdateKeyFrameElementData(keyFrameElements_[type]);
    // アニメーションの長さを更新
    UpdateDuration();
}

void KeyFrameAnimation::SetCurrentTime(float time) {
    if (time < 0.0f) {
        time = 0.0f;
    } else if (time > duration_) {
        if (isLoop_) {
            time = fmod(time, duration_);
        } else {
            time = duration_;
        }
    }
    currentTime_ = time;
}

void KeyFrameAnimation::ResetKeyFrames() {
    keyFrameElements_ = {
        { KeyFrameElementType::kPositionX, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kPositionY, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kPositionZ, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kRotationX, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kRotationY, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kRotationZ, KeyFrameElementData(KeyFrame(0.0f, 0.0f)) },
        { KeyFrameElementType::kScaleX,    KeyFrameElementData(KeyFrame(0.0f, 1.0f)) },
        { KeyFrameElementType::kScaleY,    KeyFrameElementData(KeyFrame(0.0f, 1.0f)) },
        { KeyFrameElementType::kScaleZ,    KeyFrameElementData(KeyFrame(0.0f, 1.0f)) },
        { KeyFrameElementType::kColorR,    KeyFrameElementData(KeyFrame(0.0f, 255.0f)) },
        { KeyFrameElementType::kColorG,    KeyFrameElementData(KeyFrame(0.0f, 255.0f)) },
        { KeyFrameElementType::kColorB,    KeyFrameElementData(KeyFrame(0.0f, 255.0f)) },
        { KeyFrameElementType::kColorA,    KeyFrameElementData(KeyFrame(0.0f, 255.0f)) }
    };
    
    currentKeyFrameValue_ = {
        { KeyFrameElementType::kPositionX, &keyFrameElements_[KeyFrameElementType::kPositionX].currentValue },
        { KeyFrameElementType::kPositionY, &keyFrameElements_[KeyFrameElementType::kPositionY].currentValue },
        { KeyFrameElementType::kPositionZ, &keyFrameElements_[KeyFrameElementType::kPositionZ].currentValue },
        { KeyFrameElementType::kRotationX, &keyFrameElements_[KeyFrameElementType::kRotationX].currentValue },
        { KeyFrameElementType::kRotationY, &keyFrameElements_[KeyFrameElementType::kRotationY].currentValue },
        { KeyFrameElementType::kRotationZ, &keyFrameElements_[KeyFrameElementType::kRotationZ].currentValue },
        { KeyFrameElementType::kScaleX,    &keyFrameElements_[KeyFrameElementType::kScaleX].currentValue },
        { KeyFrameElementType::kScaleY,    &keyFrameElements_[KeyFrameElementType::kScaleY].currentValue },
        { KeyFrameElementType::kScaleZ,    &keyFrameElements_[KeyFrameElementType::kScaleZ].currentValue },
        { KeyFrameElementType::kColorR,    &keyFrameElements_[KeyFrameElementType::kColorR].currentValue },
        { KeyFrameElementType::kColorG,    &keyFrameElements_[KeyFrameElementType::kColorG].currentValue },
        { KeyFrameElementType::kColorB,    &keyFrameElements_[KeyFrameElementType::kColorB].currentValue },
        { KeyFrameElementType::kColorA,    &keyFrameElements_[KeyFrameElementType::kColorA].currentValue }
    };
}

void KeyFrameAnimation::UpdateKeyFrameElementData(KeyFrameElementData &elementData) {
    if (elementData.keyFrames.size() < 2) {
        return;
    }

    // 現在の時間に最も近いキーを見つける
    auto it = std::lower_bound(
        elementData.keyFrames.begin(),
        elementData.keyFrames.end(),
        currentTime_,
        [](const KeyFrame &kf, float time) { return kf.timeSec < time; }
    );

    if (it == elementData.keyFrames.begin()) {
        elementData.currentKeyFrameIndex = 0;
        if (currentTime_ <= it->timeSec) {
            elementData.currentValue = it->value;
            return;
        }

    } else if (it == elementData.keyFrames.end()) {
        elementData.currentKeyFrameIndex = elementData.keyFrames.size() - 1;
        elementData.currentValue = it[-1].value;
        return;
    }

    it--;
    elementData.currentKeyFrameIndex = static_cast<size_t>(it - elementData.keyFrames.begin());

    const KeyFrame &kf1 = *it;
    const KeyFrame &kf2 = it[+1];
    float t = (currentTime_ - kf1.timeSec) / (kf2.timeSec - kf1.timeSec);
    elementData.currentValue = kf1.easingFunction(t, kf1.value, kf2.value);
}

void KeyFrameAnimation::UpdateDuration() {
    float maxTime = 0.0f;
    std::vector<float> lastTimes;

    // 各要素の最後のキーの時間を取得
    for (const auto &pair : keyFrameElements_) {
        const auto &element = pair.second;
        if (!element.keyFrames.empty()) {
            lastTimes.push_back(element.keyFrames.back().timeSec);
        }
    }
    // 最大の時間を見つける
    for (float time : lastTimes) {
        if (time > maxTime) {
            maxTime = time;
        }
    }
    duration_ = maxTime;
}

void KeyFrameAnimation::SortKeyFrames(KeyFrameElementData &elementData) {
    std::sort(elementData.keyFrames.begin(), elementData.keyFrames.end(), TimeCompare);
    // アニメーションの長さを更新
    UpdateDuration();
}

}