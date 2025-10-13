#pragma once
#include "Common/Easings.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace KashipanEngine {

enum class KeyFrameElementType {
    kPositionX, // X座標
    kPositionY, // Y座標
    kPositionZ, // Z座標
    kRotationX, // X軸回転
    kRotationY, // Y軸回転
    kRotationZ, // Z軸回転
    kScaleX,    // X軸スケール
    kScaleY,    // Y軸スケール
    kScaleZ,    // Z軸スケール
    kColorR,    // 赤色成分
    kColorG,    // 緑色成分
    kColorB,    // 青色成分
    kColorA,    // アルファ成分

    // 要素の総数
    kElementCount
};

// キーフレームの情報
struct KeyFrame {
    KeyFrame() = default;
    KeyFrame(float initialTimeSec, float initialValue, std::function<float(float, float, float)> initialEasingFunction = Ease::None) {
        timeSec = initialTimeSec;
        value = initialValue;
        easingFunction = initialEasingFunction;
    }

    float timeSec = 0.0f;   // キーフレームの時間（秒単位）
    float value = 0.0f;     // キーフレームの値（位置、回転、スケールなど）

    // キーフレームの補間関数（イージング関数）
    std::function<float(float, float, float)> easingFunction = Ease::None;
};

// キーフレームの要素データ
struct KeyFrameElementData {
    KeyFrameElementData() = default;
    KeyFrameElementData(const KeyFrame &firstKeyFrame) {
        keyFrames.push_back(firstKeyFrame);
        currentValue = firstKeyFrame.value;
    }

    // 現在のキーフレームのインデックス
    size_t currentKeyFrameIndex = 0;
    // 現在のアニメーション中の値
    float currentValue = 0.0f;
    // キーフレーム
    std::vector<KeyFrame> keyFrames;
};

class KeyFrameAnimation {
public:
    KeyFrameAnimation() {
        Reset();
    }
    
    void Update();

    void AddKeyFrame(KeyFrameElementType type, const KeyFrame &keyFrame);
    void RemoveKeyFrame(KeyFrameElementType type, size_t index);

    void Play() {
        isPlaying_ = true;
        currentTime_ = 0.0f;
    }
    void Stop() {
        isPlaying_ = false;
        currentTime_ = 0.0f;
    }
    void Resume() {
        isPlaying_ = true;
    }
    void Pause() {
        isPlaying_ = false;
    }

    void SetKeyFrameElementData(KeyFrameElementType type, const KeyFrameElementData &data);
    void SetKeyFrame(KeyFrameElementType type, size_t index, const KeyFrame &keyFrame);

    void SetCurrentTime(float time);
    void SetPlaySpeed(float speed) {
        playSpeed_ = speed;
    }
    void SetLoop(bool isLoop) {
        isLoop_ = isLoop;
    }

    const std::unordered_map<KeyFrameElementType, KeyFrameElementData> &GetKeyFrameElements() const {
        return keyFrameElements_;
    }
    const std::unordered_map<KeyFrameElementType, float*> &GetCurrentKeyFrameValues() const {
        return currentKeyFrameValue_;
    }

    const KeyFrameElementData &GetKeyFrameElementData(KeyFrameElementType type) const {
        return keyFrameElements_.at(type);
    }
    const float &GetCurrentKeyFrameValue(KeyFrameElementType type) {
        return *currentKeyFrameValue_.at(type);
    }

    const KeyFrame &GetKeyFrame(KeyFrameElementType type, size_t index) const {
        return keyFrameElements_.at(type).keyFrames.at(index);
    }
    const float &GetCurrentKeyFrameValue(KeyFrameElementType type) const {
        return *currentKeyFrameValue_.at(type);
    }

    float GetDuration() const {
        return duration_;
    }
    float GetCurrentTime() const {
        return currentTime_;
    }

    bool IsPlaying() const {
        return isPlaying_;
    }
    bool IsLoop() const {
        return isLoop_;
    }
    
    void Reset() {
        ResetKeyFrames();
        duration_ = 0.0f;
        currentTime_ = 0.0f;
    }

protected:
    // キーフレーム初期化用関数
    void ResetKeyFrames();
    // キーフレームの要素の更新
    void UpdateKeyFrameElementData(KeyFrameElementData &elementData);
    // アニメーションの総時間の更新
    void UpdateDuration();
    // キーフレームの順番ソート用関数
    void SortKeyFrames(KeyFrameElementData &elementData);

    // キーフレームの各要素
    std::unordered_map<KeyFrameElementType, KeyFrameElementData> keyFrameElements_;
    // 現在のキーフレームの値
    std::unordered_map<KeyFrameElementType, float *> currentKeyFrameValue_;

    // アニメーションの総時間
    float duration_ = 0.0f;
    // 現在の時間
    float currentTime_ = 0.0f;
    // 再生速度
    float playSpeed_ = 1.0f;

    // 再生フラグ
    bool isPlaying_ = false;
    // ループフラグ
    bool isLoop_ = false;
};

} // namespace KashipanEngine 