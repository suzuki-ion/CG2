#include "CloseRectToCenterY.h"
#include <Common/Easings.h>

using namespace KashipanEngine;

CloseRectToCenterY::CloseRectToCenterY(float duration, float width, float height)
    : BaseAnimation(duration) {
    easingFunction_ = Ease::OutCubic;

    width_ = width;
    height_ = height;

    greenTopPosY_ = 0.0f;
    greenBottomPosY_ = height_;
    blackTopPosY_ = 0.0f;
    blackBottomPosY_ = height_;

    startAnimationTimeToBlack_ = duration_ * 0.3f;

    topRectGreen_ = std::make_unique<Sprite>("Resources/white1x1.png");
    bottomRectGreen_ = std::make_unique<Sprite>("Resources/white1x1.png");
    topRectBlack_ = std::make_unique<Sprite>("Resources/white1x1.png");
    bottomRectBlack_ = std::make_unique<Sprite>("Resources/white1x1.png");

    auto topRectGreenState = topRectGreen_->GetStatePtr();
    topRectGreenState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);

    auto bottomRectGreenState = bottomRectGreen_->GetStatePtr();
    bottomRectGreenState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, height_, 0.0f, 1.0f);

    auto topRectBlackState = topRectBlack_->GetStatePtr();
    topRectBlackState.material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);

    auto bottomRectBlackState = bottomRectBlack_->GetStatePtr();
    bottomRectBlackState.material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, height_, 0.0f, 1.0f);
}

void CloseRectToCenterY::Update() {
    UpdateElapsedTime();

    float halfHeight = height_ / 2.0f;
    float progressGreen = elapsedTime_ / duration_;
    greenTopPosY_ = easingFunction_(progressGreen, 0.0f, halfHeight);
    greenBottomPosY_ = easingFunction_(progressGreen, height_, halfHeight);
    if (elapsedTime_ >= startAnimationTimeToBlack_) {
        float progressBlack = (elapsedTime_ - startAnimationTimeToBlack_) / (duration_ - startAnimationTimeToBlack_);
        blackTopPosY_ = easingFunction_(progressBlack, 0.0f, halfHeight);
        blackBottomPosY_ = easingFunction_(progressBlack, height_, halfHeight);
    } else {
        blackTopPosY_ = 0.0f;
        blackBottomPosY_ = height_;
    }

    CalculateTopRectGreenVertexPos();
    CalculateBottomRectGreenVertexPos();
    CalculateTopRectBlackVertexPos();
    CalculateBottomRectBlackVertexPos();
}

void CloseRectToCenterY::Draw() {
    topRectGreen_->Draw();
    bottomRectGreen_->Draw();
    topRectBlack_->Draw();
    bottomRectBlack_->Draw();
}

void CloseRectToCenterY::CalculateTopRectGreenVertexPos() {
    auto topRectGreenState = topRectGreen_->GetStatePtr();
    topRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, greenTopPosY_, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, greenTopPosY_, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
}

void CloseRectToCenterY::CalculateBottomRectGreenVertexPos() {
    auto bottomRectGreenState = bottomRectGreen_->GetStatePtr();
    bottomRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, greenBottomPosY_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, greenBottomPosY_, 0.0f, 1.0f);
}

void CloseRectToCenterY::CalculateTopRectBlackVertexPos() {
    auto topRectBlackState = topRectBlack_->GetStatePtr();
    topRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, blackTopPosY_, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, blackTopPosY_, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
}

void CloseRectToCenterY::CalculateBottomRectBlackVertexPos() {
    auto bottomRectBlackState = bottomRectBlack_->GetStatePtr();
    bottomRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, blackBottomPosY_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, blackBottomPosY_, 0.0f, 1.0f);
}
