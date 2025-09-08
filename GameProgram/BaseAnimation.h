#pragma once

class BaseAnimation {
    public:
    BaseAnimation(float duration) : duration_(duration) {}
    virtual ~BaseAnimation() = default;
    
    void Play() {
        isPlaying_ = true;
        isFinished_ = false;
        isFinishedTrigger_ = false;
        elapsedTime_ = 0.0f;
    }
    void Stop() {
        isPlaying_ = false;
        isFinished_ = false;
        isFinishedTrigger_ = false;
        elapsedTime_ = 0.0f;
    }
    void Pause() {
        isPlaying_ = false;
    }
    void Resume() {
        if (!isFinished_) {
            isPlaying_ = true;
        }
    }
    void End() {
        isPlaying_ = false;
        isFinished_ = true;
        isFinishedTrigger_ = true;
        elapsedTime_ = duration_;
    }

    float GetDuration() const {
        return duration_;
    }
    float GetElapsedTime() const {
        return elapsedTime_;
    }
    float GetProgress() const {
        return duration_ > 0.0f ? (elapsedTime_ / duration_) : 1.0f;
    }

    bool IsPlaying() const {
        return isPlaying_;
    }
    bool IsFinished() const {
        return isFinished_;
    }
    bool IsFinishedTrigger() const {
        return isFinishedTrigger_;
    }

    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:
    void UpdateElapsedTime();
    float duration_ = 0.0f;
    float elapsedTime_ = 0.0f;
    bool isPlaying_ = false;
    bool isFinished_ = false;
    bool isFinishedTrigger_ = false;
};