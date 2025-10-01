#pragma once
#include <Objects/Sprite.h>
#include <Objects/Text.h>
#include <Common/Easings.h>
#include <functional>

class PlayerHealthBar {
public:
    PlayerHealthBar();
    ~PlayerHealthBar() = default;

    void Update();
    void Draw();

    void SetHealthMax(float healthMax);
    void SetHealthCurrent(float healthCurrent);

    void SetPosition(const KashipanEngine::Vector2 &position) { position_ = position; }
    void SetWidth(float width) { width_ = width; }
    void SetHeight(float height) { height_ = height; }
    void SetOutlineThickness(float thickness) { outlineThickness_ = thickness; }
    void SetOuterEdgeThickness(float thickness) { outerEdgeThickness_ = thickness; }

    float GetHealthMax() const { return healthMax_; }
    float GetHealthCurrent() const { return healthCurrent_; }

private:
    void CalculateOutlineSpriteVertexPos();
    void CalculateOuterEdgeSpriteVertexPos();
    void CalculateCurrentHealthSpriteVertexPos();
    void CalculateLostHealthSpriteVertexPos();
    void CalculateHealthTextPos();
    void CalculateHealthColor();
    void SetHealthText();

    void AnimationUpdate();

    // HPバーのワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // テキスト用のワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> textWorldTransform_;

    // シェイクアニメーション用のワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> shakeWorldTransform_;

    float healthMax_ = 100.0f;
    float healthCurrent_ = 100.0f;

    // 失った体力(アニメーション用)
    float healthLost_ = 0.0f;
    // 失った体力(アニメーション開始前の値)
    float healthLostStart_ = 0.0f;
    // 失った体力(アニメーション終了後の値)
    float healthLostEnd_ = 0.0f;

    // 失った体力が増えたときにアニメーションを開始するまでの時間(秒)
    float healthLostAnimationDelay_ = 0.5f;
    // 失った体力が減りきるまでの時間(秒)
    float healthLostAnimationDuration_ = 0.5f;
    // アニメーションの経過時間(秒)
    float healthLostAnimationTimer_ = 0.0f;
    // アニメーションフラグ
    bool isHealthLostAnimation_ = false;
    // アニメーションに使用するイージング関数
    std::function<float(float, float, float)> healthLostEasingFunction_ = KashipanEngine::Ease::OutCubic;

    // ゲージの位置(左上)(枠線を含む)
    KashipanEngine::Vector2 position_ = { 16.0f, 16.0f };
    float width_ = 512.0f;  // ゲージの幅(枠線を含まない)
    float height_ = 64.0f;  // ゲージの高さ(枠線を含まない)
    float outlineThickness_ = 4.0f;
    float outerEdgeThickness_ = 8.0f;

    // HPバーの色(体力MAXの時)
    KashipanEngine::Vector4 healthColorMax_ = { 0.0f, 1.0f, 0.0f, 1.0f };
    // HPバーの色(体力0の時)
    KashipanEngine::Vector4 healthColorMin_ = { 1.0f, 0.0f, 0.0f, 1.0f };
    // 現在のHPバーの色
    KashipanEngine::Vector4 healthColor_ = { 0.0f, 1.0f, 0.0f, 1.0f };

    std::unique_ptr<KashipanEngine::Sprite> outlineSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> outerEdgeSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> currentHealthSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Sprite> lostHealthSprite_ = nullptr;
    std::unique_ptr<KashipanEngine::Text> healthTextGreen_ = nullptr;
    std::unique_ptr<KashipanEngine::Text> healthTextRed_ = nullptr;
};