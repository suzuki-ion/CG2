#pragma once
#include <Objects.h>

class BaseParticle {
public:
    BaseParticle() = delete;
    BaseParticle(
        KashipanEngine::Model *model,
        const KashipanEngine::Vector3 &position,
        const KashipanEngine::Vector3 &direction,
        const KashipanEngine::Vector3 &rotation,
        float speed,
        float rotationSpeed,
        float lifeTime = 0.0f,
        bool isEraseByLife = false
    );
    virtual ~BaseParticle() = default;

    void SetPosition(const KashipanEngine::Vector3 &position) { worldTransform_->translate_ = position; }
    void SetRotation(const KashipanEngine::Vector3 &rotation) { worldTransform_->rotate_ = rotation; }
    void SetScale(const KashipanEngine::Vector3 &scale) { worldTransform_->scale_ = scale; }
    const KashipanEngine::Vector3 &GetPosition() const { return worldTransform_->translate_; }
    const KashipanEngine::Vector3 &GetRotation() const { return worldTransform_->rotate_; }
    const KashipanEngine::Vector3 &GetScale() const { return worldTransform_->scale_; }

    const KashipanEngine::Vector3 &GetDirection() const { return direction_; }
    const KashipanEngine::Vector3 &GetRotationDirection() const { return rotation_; }

    bool IsAlive() const { return isAlive_; }

    virtual void Update() { UpdateCommon(); }
    virtual void Draw() { DrawCommon(); }

protected:
    void UpdateCommon();
    void DrawCommon();

    // パーティクルのモデル
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;

    // パーティクルの移動方向
    KashipanEngine::Vector3 direction_ = { 0.0f, 0.0f, 0.0f };
    // パーティクルの回転方向
    KashipanEngine::Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };

    // パーティクルの移動速度
    float speed_ = 0.0f;
    // パーティクルの回転速度
    float rotationSpeed_ = 0.0f;

    // パーティクルの寿命(秒)
    float lifeTime_ = 0.0f;
    // パーティクルの経過時間(秒)
    float elapsedTime_ = 0.0f;
    // パーティクルの生存フラグ
    bool isAlive_ = true;
    // パーティクルを寿命で消すかどうか
    bool isEraseByLife_ = false;
};