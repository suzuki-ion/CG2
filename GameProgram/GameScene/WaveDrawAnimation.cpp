#include "WaveDrawAnimation.h"
#include <KashipanEngine.h>
#include <Common/Easings.h>
#include <Common/Random.h>

using namespace KashipanEngine;

WaveDrawAnimation::WaveDrawAnimation(float width, float height) 
    : BaseAnimation(4.0f) {
    width_ = width;
    height_ = height;

    textWorldTransform_ = std::make_unique<WorldTransform>();
    textShakeWorldTransform_ = std::make_unique<WorldTransform>();
    textWorldTransform_->parentTransform_ = textShakeWorldTransform_.get();
    textWorldTransform_->translate_ = Vector3(width_ / 2.0f, height_ / 2.0f, 0.0f);

    rectSpriteTop_ = std::make_unique<Sprite>("Resources/white1x1.png");
    rectSpriteBottom_ = std::make_unique<Sprite>("Resources/white1x1.png");

    waveText_ = std::make_unique<Text>(32);
    waveText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    waveText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    Reset();
}

void WaveDrawAnimation::SetWaveNumber(int waveNumber, bool isLastWave, bool isClearWave) {
    waveNumber_ = waveNumber;
    isLastWave_ = isLastWave;
    isClearWave_ = isClearWave;
    Reset();
}

void WaveDrawAnimation::Update() {
    prevElapsedTime_ = elapsedTime_;
    UpdateElapsedTime();
    if (!isPlaying_) {
        return;
    }
    textShakeWorldTransform_->TransferMatrix();

    auto topState = rectSpriteTop_->GetStatePtr();
    auto bottomState = rectSpriteBottom_->GetStatePtr();
    auto textState = waveText_->GetStatePtr();

    noneTime_ = duration_ - 3.0f;
    if (noneTime_ < 0.0f) {
        noneTime_ = 0.0f;
    }
    const float spawnedRectTime = noneTime_ + 0.5f;
    const float openedRectTime = noneTime_ + 1.0f;
    const float shakedTextTime = noneTime_ + 2.0f;

    float halfHeight = height_ / 2.0f;
    float closedTopPosY = halfHeight - 128.0f;
    float closedBottomPosY = halfHeight + 128.0f;
    float closedHeight = 32.0f;

    if (elapsedTime_ >= noneTime_ && prevElapsedTime_ < noneTime_) {
        isAnimationStartTrigger_ = true;
    } else {
        isAnimationStartTrigger_ = false;
    }

    if (elapsedTime_ < noneTime_) {
    } else if (elapsedTime_ < spawnedRectTime) {
        topState.material->color.w = 1.0f;
        bottomState.material->color.w = 1.0f;
        textState.material->color.w = 1.0f;

        float progress = (elapsedTime_ - noneTime_) / (spawnedRectTime - noneTime_);
        topState.mesh->vertexBufferMap[1].position.y = Ease::OutCubic(progress, halfHeight, closedTopPosY);
        topState.mesh->vertexBufferMap[3].position.y = Ease::OutCubic(progress, halfHeight, closedTopPosY);
        bottomState.mesh->vertexBufferMap[0].position.y = Ease::OutCubic(progress, halfHeight, closedBottomPosY);
        bottomState.mesh->vertexBufferMap[2].position.y = Ease::OutCubic(progress, halfHeight, closedBottomPosY);

        topState.mesh->vertexBufferMap[0].position.y = halfHeight;
        topState.mesh->vertexBufferMap[2].position.y = halfHeight;
        bottomState.mesh->vertexBufferMap[1].position.y = halfHeight;
        bottomState.mesh->vertexBufferMap[3].position.y = halfHeight;

    } else if (elapsedTime_ < openedRectTime) {
        float progress = (elapsedTime_ - spawnedRectTime) / (openedRectTime - spawnedRectTime);
        waveText_->Show();
        topState.mesh->vertexBufferMap[0].position.y = Ease::OutCubic(progress, halfHeight, closedTopPosY + closedHeight);
        topState.mesh->vertexBufferMap[2].position.y = Ease::OutCubic(progress, halfHeight, closedTopPosY + closedHeight);
        bottomState.mesh->vertexBufferMap[1].position.y = Ease::OutCubic(progress, halfHeight, closedBottomPosY - closedHeight);
        bottomState.mesh->vertexBufferMap[3].position.y = Ease::OutCubic(progress, halfHeight, closedBottomPosY - closedHeight);

    } else if (elapsedTime_ < shakedTextTime) {
        float progress = (elapsedTime_ - openedRectTime) / (shakedTextTime - openedRectTime);
        if (isClearWave_) {
            waveText_->SetText(u8"WAVE ALL CLEAR");
        } else if (isLastWave_) {
            waveText_->SetText(u8"WAVE LAST");
        } else {
            std::string text = "WAVE " + std::to_string(waveNumber_);
            waveText_->SetText(std::u8string(text.begin(), text.end()));
        }
        float shakeAmount = 8.0f;
        textShakeWorldTransform_->translate_ = Vector3(
            GetRandomFloat(-shakeAmount, shakeAmount) * (1.0f - progress),
            GetRandomFloat(-shakeAmount, shakeAmount) * (1.0f - progress),
            0.0f
        );

    } else {
        float progress = (elapsedTime_ - shakedTextTime) / (duration_ - shakedTextTime);
        topState.material->color.w = 1.0f - progress;
        bottomState.material->color.w = 1.0f - progress;
        textState.material->color.w = 1.0f - progress;
    }

    if (isLastWave_ && !isClearWave_) {
        if (std::fmodf(elapsedTime_, 0.5f) < 0.25f) {
            textState.material->color.x = 1.0f;
            textState.material->color.y = 1.0f;
            textState.material->color.z = 1.0f;
        } else {
            textState.material->color.x = 1.0f;
            textState.material->color.y = 0.0f;
            textState.material->color.z = 0.0f;
        }
    }
}

void WaveDrawAnimation::Draw() {
    if (!isPlaying_) {
        return;
    }
    waveText_->SetPipelineName("Object3d.Solid.BlendNormal");
    waveText_->Draw(*textWorldTransform_);
    waveText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    waveText_->Draw(*textWorldTransform_);
    rectSpriteTop_->Draw();
    rectSpriteBottom_->Draw();
}

void WaveDrawAnimation::Reset() {
    elapsedTime_ = 0.0f;
    prevElapsedTime_ = 0.0f;
    isPlaying_ = false;
    isAnimationStartTrigger_ = false;

    waveText_->SetText(u8"WAVE");
    waveText_->Hide();
    auto topState = rectSpriteTop_->GetStatePtr();
    auto bottomState = rectSpriteBottom_->GetStatePtr();
    auto textState = waveText_->GetStatePtr();
    float halfHeight = height_ / 2.0f;
    topState.mesh->vertexBufferMap[0].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    topState.mesh->vertexBufferMap[1].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    topState.mesh->vertexBufferMap[2].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    topState.mesh->vertexBufferMap[3].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    bottomState.mesh->vertexBufferMap[0].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    bottomState.mesh->vertexBufferMap[1].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    bottomState.mesh->vertexBufferMap[2].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    bottomState.mesh->vertexBufferMap[3].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    bottomState.material->color.w = 1.0f;
    textState.material->color.w = 1.0f;
    textShakeWorldTransform_->translate_ = Vector3(0.0f);

    if (isClearWave_) {
        ResetClear();
    } else if (isLastWave_) {
        ResetLast();
    } else {
        ResetNormal();
    }
}

void WaveDrawAnimation::ResetNormal() {
    duration_ = 4.0f;
    waveText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    rectSpriteTop_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    rectSpriteBottom_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void WaveDrawAnimation::ResetLast() {
    duration_ = 5.0f;
    waveText_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    rectSpriteTop_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    rectSpriteBottom_->GetStatePtr().material->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

void WaveDrawAnimation::ResetClear() {
    duration_ = 5.0f;
    waveText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    rectSpriteTop_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    rectSpriteBottom_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}
