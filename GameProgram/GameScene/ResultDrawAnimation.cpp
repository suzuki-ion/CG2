#include "ResultDrawAnimation.h"
#include <Common/Easings.h>
#include <format>

using namespace KashipanEngine;

ResultDrawAnimation::ResultDrawAnimation(float width, float height)
    : BaseAnimation(6.0f) {
    width_ = width;
    height_ = height;
    resultLogoText_ = std::make_unique<Text>(16);
    killCountText_ = std::make_unique<Text>(64);
    totalDamageText_ = std::make_unique<Text>(64);
    damageTakenText_ = std::make_unique<Text>(64);

    resultLogoText_->SetFont("Resources/Font/jfdot-k14_128.fnt");
    resultLogoText_->SetText(u8"RESULT");
    killCountText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    totalDamageText_->SetFont("Resources/Font/jfdot-k14_64.fnt");
    damageTakenText_->SetFont("Resources/Font/jfdot-k14_64.fnt");

    resultLogoText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    killCountText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    totalDamageText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);
    damageTakenText_->SetTextAlign(TextAlignX::Center, TextAlignY::Center);

    resultLogoText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    killCountText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    totalDamageText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    damageTakenText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);

    resultLogoText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 5.0f, 0.0f);
    killCountText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f - 96.0f, 0.0f);
    totalDamageText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f, 0.0f);
    damageTakenText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f + 96.0f, 0.0f);
}

void ResultDrawAnimation::SetResultParameters(const ResultParameters &params) {
    params_ = params;
    Reset();
}

void ResultDrawAnimation::Update() {
    UpdateElapsedTime();

    const float logoFadeInStartTime = 1.0f;
    const float logoFadeInEndTime = 2.0f;

    const float killCountFadeInStartTime = 3.0f;
    const float killCountFadeInEndTime = 4.0f;

    const float totalDamageFadeInStartTime = 3.5f;
    const float totalDamageFadeInEndTime = 4.5f;

    const float damageTakenFadeInStartTime = 4.0f;
    const float damageTakenFadeInEndTime = 5.0f;

    const float fadeInPosYDown = 32.0f;

    if (elapsedTime_ >= logoFadeInStartTime && elapsedTime_ <= logoFadeInEndTime) {
        float t = (elapsedTime_ - logoFadeInStartTime) / (logoFadeInEndTime - logoFadeInStartTime);
        resultLogoText_->GetStatePtr().material->color.w = t;
        resultLogoText_->GetStatePtr().transform->translate.y = Ease::OutCubic(t, height_ / 5.0f - fadeInPosYDown, height_ / 5.0f);
    }

    if (elapsedTime_ >= killCountFadeInStartTime && elapsedTime_ <= killCountFadeInEndTime) {
        float t = (elapsedTime_ - killCountFadeInStartTime) / (killCountFadeInEndTime - killCountFadeInStartTime);
        killCountText_->GetStatePtr().material->color.w = t;
        killCountText_->GetStatePtr().transform->translate.y = Ease::OutCubic(t, height_ / 2.0f - 96.0f - fadeInPosYDown, height_ / 2.0f - 96.0f);
    }

    if (elapsedTime_ >= totalDamageFadeInStartTime && elapsedTime_ <= totalDamageFadeInEndTime) {
        float t = (elapsedTime_ - totalDamageFadeInStartTime) / (totalDamageFadeInEndTime - totalDamageFadeInStartTime);
        totalDamageText_->GetStatePtr().material->color.w = t;
        totalDamageText_->GetStatePtr().transform->translate.y = Ease::OutCubic(t, height_ / 2.0f - fadeInPosYDown, height_ / 2.0f);
    }

    if (elapsedTime_ >= damageTakenFadeInStartTime && elapsedTime_ <= damageTakenFadeInEndTime) {
        float t = (elapsedTime_ - damageTakenFadeInStartTime) / (damageTakenFadeInEndTime - damageTakenFadeInStartTime);
        damageTakenText_->GetStatePtr().material->color.w = t;
        damageTakenText_->GetStatePtr().transform->translate.y = Ease::OutCubic(t, height_ / 2.0f + 96.0f - fadeInPosYDown, height_ / 2.0f + 96.0f);
    }
}

void ResultDrawAnimation::Draw() {
    if (!isPlaying_ && !isFinished_) {
        return;
    }
    resultLogoText_->SetPipelineName("Object3d.Solid.BlendNormal");
    resultLogoText_->Draw();
    resultLogoText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    resultLogoText_->Draw();

    killCountText_->SetPipelineName("Object3d.Solid.BlendNormal");
    killCountText_->Draw();
    killCountText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    killCountText_->Draw();

    totalDamageText_->SetPipelineName("Object3d.Solid.BlendNormal");
    totalDamageText_->Draw();
    totalDamageText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    totalDamageText_->Draw();

    damageTakenText_->SetPipelineName("Object3d.Solid.BlendNormal");
    damageTakenText_->Draw();
    damageTakenText_->SetPipelineName("Object3d.Solid.BlendMultiply");
    damageTakenText_->Draw();
}

void ResultDrawAnimation::Reset() {
    std::string killCountStr = std::format("Kill Count: {}", params_.killCount);
    killCountText_->SetText(std::u8string(killCountStr.begin(), killCountStr.end()));
    std::string totalDamageStr = std::format("Total Damage: {:.2f}", params_.totalDamage);
    totalDamageText_->SetText(std::u8string(totalDamageStr.begin(), totalDamageStr.end()));
    std::string damageTakenStr = std::format("Damage Taken: {:.2f}", params_.damageTaken);
    damageTakenText_->SetText(std::u8string(damageTakenStr.begin(), damageTakenStr.end()));

    resultLogoText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    killCountText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    totalDamageText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    damageTakenText_->GetStatePtr().material->color = Vector4(0.0f, 1.0f, 0.0f, 0.0f);

    resultLogoText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 4.0f, 0.0f);
    killCountText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f - 96.0f, 0.0f);
    totalDamageText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f, 0.0f);
    damageTakenText_->GetStatePtr().transform->translate = Vector3(width_ / 2.0f, height_ / 2.0f + 96.0f, 0.0f);
}