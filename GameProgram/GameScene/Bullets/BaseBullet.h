#pragma once
#include <KashipanEngine.h>
#include <Math/Vector3.h>
#include <Objects/Model.h>
#include "../Collider.h"
#include "../CollisionConfig.h"

#include <unordered_map>
#include <string>
#include <functional>

// 弾の種類
enum class BulletType {
    kNone,      // 無効
    kNormal,    // 通常弾
    kHoming,    // ホーミング弾
    kPenetrate, // 貫通弾
    kCount      // 弾の種類の数
};

const std::unordered_map<std::string, BulletType> kBulletTypeMap = {
    { "None", BulletType::kNone },
    { "Normal", BulletType::kNormal },
    { "Homing", BulletType::kHoming },
    { "Penetrate", BulletType::kPenetrate }
};

class BaseEntity;

class BaseBullet : public Collider {
public:
    BaseBullet() = delete;
    BaseBullet(KashipanEngine::Model *bulletModel, const std::bitset<8> &collisionAttributes,
        float radius, float attackDefault, float speedDefault, float intervalDefault, float attack, float speed, float interval) {
        SetCollisionAttribute(collisionAttributes);
        SetCollisionMask(std::bitset<8>(collisionAttributes).flip());
        SetRadius(radius);
        bulletModel_ = bulletModel;
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

    void SetGiveExpFunc(const std::function<void(float)> &func) { giveExpFunc_ = func; }
    void SetPosition(const KashipanEngine::Vector3 &pos) { worldTransform_->translate_ = pos; }
    void SetModel(KashipanEngine::Model *model) { bulletModel_ = model; }
    void SetDirection(const KashipanEngine::Vector3 &dir) { direction_ = dir; }
    void SetTarget(BaseEntity *target) { target_ = target; }
    void SetIsExistTargetFunc(const std::function<bool(BaseEntity *)> &func) { isExistTargetFunc_ = func; }
    void SetShowDamageFunction(std::function<void(float, const KashipanEngine::Vector3 &)> showDamageFunc) { showDamageFunc_ = showDamageFunc; }
    void SetSpeed(float speed) { speed_ = speed; }
    
    const KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    const KashipanEngine::Vector3 &GetPosition() const { return worldTransform_->translate_; }
    const KashipanEngine::Vector3 &GetDirection() const { return direction_; }
    float GetAttack() const { return attack_; }
    float GetSpeed() const { return speed_; }
    float GetInterval() const { return interval_; }

    bool IsAlive() const { return isAlive_; }

protected:
    /// @brief ターゲットのポインタが有効か確認する
    /// @return 有効ならtrue、無効ならfalseを返す
    bool IsExistTarget();
    /// @brief 進行方向に回転を合わせる
    void RotationToDirection();

    // プレイヤーに経験値を与える用の関数ポインタ
    std::function<void(float)> giveExpFunc_ = nullptr;

    // 弾のデフォルトの攻撃力
    float attackDefault_ = 1.0f;
    // 実際の攻撃力
    float attack_ = 1.0f;

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

    // 弾のモデルへのポインタ
    KashipanEngine::Model *bulletModel_ = nullptr;
    // 弾の発射方向
    KashipanEngine::Vector3 direction_ = KashipanEngine::Vector3(0.0f);

    // ターゲットへのポインタ
    BaseEntity *target_ = nullptr;
    // ターゲットが存在するかの確認用関数ポインタ
    std::function<bool(BaseEntity *)> isExistTargetFunc_ = nullptr;

    // ダメージ量表示用関数ポインタ
    std::function<void(float, const KashipanEngine::Vector3 &)> showDamageFunc_ = nullptr;

    // ターゲットに向かって飛ぶかどうか
    bool isHoming_ = false;
    // 生存フラグ
    bool isAlive_ = true;
};