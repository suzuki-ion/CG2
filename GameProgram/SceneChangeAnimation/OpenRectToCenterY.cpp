#include "OpenRectToCenterY.h"
#include <Common/Easings.h>

using namespace KashipanEngine;

OpenRectToCenterY::OpenRectToCenterY(float duration, float width, float height)
    : BaseAnimation(duration) {
    easingFunction_ = Ease::InCubic;

    width_ = width;
    height_ = height;

    float halfHeight = height_ / 2.0f;

    greenTopPosY_ = halfHeight;
    greenBottomPosY_ = halfHeight;
    blackTopPosY_ = halfHeight;
    blackBottomPosY_ = halfHeight;

    startAnimationTimeToGreen_ = duration_ * 0.5f;

    topRectGreen_ = std::make_unique<Sprite>("Resources/white1x1.png");
    bottomRectGreen_ = std::make_unique<Sprite>("Resources/white1x1.png");
    topRectBlack_ = std::make_unique<Sprite>("Resources/white1x1.png");
    bottomRectBlack_ = std::make_unique<Sprite>("Resources/white1x1.png");

    auto topRectGreenState = topRectGreen_->GetStatePtr();
    topRectGreenState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);

    auto bottomRectGreenState = bottomRectGreen_->GetStatePtr();
    bottomRectGreenState.material->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, halfHeight, 0.0f, 1.0f);

    auto topRectBlackState = topRectBlack_->GetStatePtr();
    topRectBlackState.material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);

    auto bottomRectBlackState = bottomRectBlack_->GetStatePtr();
    bottomRectBlackState.material->color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, halfHeight, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, halfHeight, 0.0f, 1.0f);
}

void OpenRectToCenterY::Update() {
    UpdateElapsedTime();

    float halfHeight = height_ / 2.0f;
    float progressBlack = elapsedTime_ / duration_;
    blackTopPosY_ = easingFunction_(progressBlack, halfHeight, 0.0f);
    blackBottomPosY_ = easingFunction_(progressBlack, halfHeight, height_);
    if (elapsedTime_ >= startAnimationTimeToGreen_) {
        float progressGreen = (elapsedTime_ - startAnimationTimeToGreen_) / (duration_ - startAnimationTimeToGreen_);
        greenTopPosY_ = easingFunction_(progressGreen, halfHeight, 0.0f);
        greenBottomPosY_ = easingFunction_(progressGreen, halfHeight, height_);
    } else {
        greenTopPosY_ = halfHeight;
        greenBottomPosY_ = halfHeight;
    }

    CalculateTopRectGreenVertexPos();
    CalculateBottomRectGreenVertexPos();
    CalculateTopRectBlackVertexPos();
    CalculateBottomRectBlackVertexPos();
}

void OpenRectToCenterY::Draw() {
    topRectGreen_->Draw();
    bottomRectGreen_->Draw();
    topRectBlack_->Draw();
    bottomRectBlack_->Draw();
}

void OpenRectToCenterY::CalculateTopRectGreenVertexPos() {
    auto topRectGreenState = topRectGreen_->GetStatePtr();
    topRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, greenTopPosY_, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, greenTopPosY_, 0.0f, 1.0f);
    topRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
}

void OpenRectToCenterY::CalculateBottomRectGreenVertexPos() {
    auto bottomRectGreenState = bottomRectGreen_->GetStatePtr();
    bottomRectGreenState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[1].position = Vector4(0.0f, greenBottomPosY_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectGreenState.mesh->vertexBufferMap[3].position = Vector4(width_, greenBottomPosY_, 0.0f, 1.0f);
}

void OpenRectToCenterY::CalculateTopRectBlackVertexPos() {
    auto topRectBlackState = topRectBlack_->GetStatePtr();
    topRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, blackTopPosY_, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, blackTopPosY_, 0.0f, 1.0f);
    topRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, 0.0f, 0.0f, 1.0f);
}

void OpenRectToCenterY::CalculateBottomRectBlackVertexPos() {
    auto bottomRectBlackState = bottomRectBlack_->GetStatePtr();
    bottomRectBlackState.mesh->vertexBufferMap[0].position = Vector4(0.0f, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[1].position = Vector4(0.0f, blackBottomPosY_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[2].position = Vector4(width_, height_, 0.0f, 1.0f);
    bottomRectBlackState.mesh->vertexBufferMap[3].position = Vector4(width_, blackBottomPosY_, 0.0f, 1.0f);
}
