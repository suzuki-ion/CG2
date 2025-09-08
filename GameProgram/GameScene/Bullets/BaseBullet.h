#pragma once
#include <KashipanEngine.h>
#include <Math/Vector3.h>
#include <Objects/Sprite.h>
#include "../Collider.h"
#include "../CollisionConfig.h"

#include <unordered_map>
#include <string>
#include <functional>

// 弾の種類
enum class BulletType {
    kNone,      // 無効
    kNormal,    // 通常弾
    kShotgun,   // 散弾
    kPenetrate, // 貫通弾
    kCount      // 弾の種類の数
};

const std::unordered_map<std::string, BulletType> kBulletTypeMap = {
    { "None", BulletType::kNone },
    { "Normal", BulletType::kNormal },
    { "Shotgun", BulletType::kShotgun },
    { "Penetrate", BulletType::kPenetrate }
};

class BaseEntity;

class BaseBullet : public Collider {
public:
    BaseBullet() = delete;
    BaseBullet(KashipanEngine::Sprite *bulletSprite, const std::bitset<8> &collisionAttributes,
        float radius, int attackDefault, float speedDefault, float intervalDefault, int attack, float speed, float interval) {
        SetCollisionAttribute(collisionAttributes);
        SetCollisionMask(std::bitset<8>(collisionAttributes).flip());
        SetRadius(radius);
        bulletSprite_ = bulletSprite;
        attackDefault_ = attackDefault;
        speedDefault_ = speedDefault;
        intervalDefault_ = intervalDefault;
        attack_ = attackDefault_ * attack;
        speed_ = speedDefault_ * speed;
        interval_ = intervalDefault_ * interval;
        worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    }
    virtual ~BaseBullet() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    void SetPosition(const KashipanEngine::Vector3 &pos) { worldTransform_->translate_ = pos; }
    void SetSprite(KashipanEngine::Sprite *sprite) { bulletSprite_ = sprite; }
    void SetDirection(const KashipanEngine::Vector3 &dir) { direction_ = dir; }
    void SetTarget(BaseEntity *target) { target_ = target; }
    void SetIsExistTargetFunc(const std::function<bool(BaseEntity *)> &func) { isExistTargetFunc_ = func; }
    void SetSpeed(float speed) { speed_ = speed; }
    
    bool CheckCollision(Collider *other) override;
    const KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    const KashipanEngine::Vector3 &GetPosition() const { return worldTransform_->translate_; }
    const KashipanEngine::Vector3 &GetDirection() const { return direction_; }
    int GetAttack() const { return attack_; }
    float GetSpeed() const { return speed_; }
    float GetInterval() const { return interval_; }

    bool IsAlive() const { return isAlive_; }

protected:
    /// @brief ターゲットのポインタが有効か確認する
    /// @return 有効ならtrue、無効ならfalseを返す
    bool IsExistTarget();
    /// @brief 進行方向に回転を合わせる
    void RotationToDirection();

    // 弾のデフォルトの攻撃力
    int attackDefault_ = 1;
    // 実際の攻撃力
    int attack_ = 1;

    // 弾のデフォルトの移動速度
    float speedDefault_ = 1.0f;
    // 実際の移動速度
    float speed_ = 1.0f;

    // 弾のデフォルトの発射間隔
    float intervalDefault_ = 1.0f;
    // 実際の発射間隔
    float interval_ = 1.0f;

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;

    // 弾のスプライトへのポインタ
    KashipanEngine::Sprite *bulletSprite_ = nullptr;
    // 弾の発射方向
    KashipanEngine::Vector3 direction_ = KashipanEngine::Vector3(0.0f);

    // ターゲットへのポインタ
    BaseEntity *target_ = nullptr;
    // ターゲットが存在するかの確認用関数ポインタ
    std::function<bool(BaseEntity *)> isExistTargetFunc_ = nullptr;

    // ターゲットに向かって飛ぶかどうか
    bool isHoming_ = false;
    // 生存フラグ
    bool isAlive_ = true;
};