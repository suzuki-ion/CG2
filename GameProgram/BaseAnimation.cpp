#include "BaseAnimation.h"
#include <KashipanEngine.h>

void BaseAnimation::UpdateElapsedTime() {
    if (!isPlaying_) {
        isFinishedTrigger_ = false;
        return;
    }
    elapsedTime_ += Engine::GetDeltaTime();
    if (elapsedTime_ >= duration_) {
        elapsedTime_ = duration_;
        isPlaying_ = false;
        isFinished_ = true;
        isFinishedTrigger_ = true;
    }
}
