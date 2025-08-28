#pragma once
#include <KashipanEngine.h>
#include <Math/Camera.h>
#include <Objects.h>
#include <list>
#include <memory>
#include <functional>

#include "Bullets/BaseBullet.h"
#include "StatusParameters.h"
#include "Collider.h"

class BaseEntity : public Collider {
public:
    enum class MoveDirectionLR {
        kMoveNone,
        kMoveLeft,
        kMoveRight
    };
    enum class MoveDirectionUD {
        kMoveNone,
        kMoveUp,
        kMoveDown
    };

    BaseEntity(KashipanEngine::Model *model, KashipanEngine::Model *bulletModel, KashipanEngine::Camera *camera);
    virtual ~BaseEntity() = default;

    virtual void SetLevel(int level) = 0;
    void SetTargetList(const std::list<BaseEntity *> &targetList);
    void SetShootBulletType(BulletType type) { shootBulletType_ = type; }
    void SetShootFunction(std::function<void(std::unique_ptr<BaseBullet> bullet)> shootFunction) {
        shootFunction_ = shootFunction;
    }
    void SetShowDamageFunction(std::function<void(float, const KashipanEngine::Vector3 &)> showDamageFunc) {
        showDamageFunc_ = showDamageFunc;
    }
    void SetResultAddDamageFunction(std::function<void(float)> resultAddDamageFunction) {
        resultAddDamageFunction_ = resultAddDamageFunction;
    }
    void SetResultAddKillCountFunction(std::function<void()> resultAddKillCountFunction) {
        resultAddKillCountFunction_ = resultAddKillCountFunction;
    }

    float GetShootIntervalTimer() const { return shootIntervalTimer_; }
    const StatusParameters &GetStatus() { return statusParametersCurrent_; }
    const KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    KashipanEngine::Vector3 GetLocalPosition() {
        return worldTransform_->translate_;
    }
    void Damage(float damage) {
        statusParametersCurrent_.healthCurrent -= damage;
        if (resultAddDamageFunction_) {
            resultAddDamageFunction_(damage);
        }
        if (statusParametersCurrent_.healthCurrent <= 0.0f) {
            statusParametersCurrent_.healthCurrent = 0.0f;
            isAlive_ = false;
            if (resultAddKillCountFunction_) {
                resultAddKillCountFunction_();
            }
        }
    }
    bool IsAlive() const { return isAlive_; }

    /// @brief 弾発射用関数
    /// @return 発射に成功したらtrueを返す
    virtual bool Shoot();

    // 更新処理
    virtual void Update() = 0;
    // 描画処理
    virtual void Draw() = 0;

protected:
    // 速度から向きを設定する関数
    void SetDirectionFromVelocity();
    // シェイクアニメーション用関数
    void ShakeAnimation();

    // デフォルトのステータスパラメータ
    StatusParameters statusParametersDefault_;
    // 現在のステータスパラメータ
    StatusParameters statusParametersCurrent_;

    // 弾発射用関数へのポインタ
    std::function<void(std::unique_ptr<BaseBullet> bullet)> shootFunction_;
    // ダメージ量表示用関数ポインタ
    std::function<void(float, const KashipanEngine::Vector3 &)> showDamageFunc_ = nullptr;
    // リザルトのダメージ加算用の関数ポインタ
    std::function<void(float)> resultAddDamageFunction_ = nullptr;
    // リザルトのキル数加算用の関数ポインタ
    std::function<void()> resultAddKillCountFunction_ = nullptr;

    // カメラへのポインタ
    KashipanEngine::Camera *camera_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // シェイク用ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> shakeWorldTransform_;
    // カメラのワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> cameraWorldTransform_;
    // 回転アニメーション専用ベクトル
    KashipanEngine::Vector3 rotateAnimationVector_;
    // 回転専用ベクトル
    KashipanEngine::Vector3 rotateVector_;

    // モデル
    KashipanEngine::Model *model_;
    // 弾のモデル
    KashipanEngine::Model *bulletModel_;

    // 速度
    KashipanEngine::Vector3 velocity_;
    // 移動方向
    KashipanEngine::Vector3 moveDirection_;
    // 移動方向(左右)
    MoveDirectionLR moveDirectionLR_ = MoveDirectionLR::kMoveNone;
    // 移動方向(上下)
    MoveDirectionUD moveDirectionUD_ = MoveDirectionUD::kMoveNone;

    // ターゲットへのポインタリスト
    std::list<BaseEntity *> targetList_;

    // 発射する弾の種類
    BulletType shootBulletType_ = BulletType::kNone;
    // 弾の発射方向
    KashipanEngine::Vector3 shootDirection_ = { 0.0f, 0.0f, 1.0f };
    // 1秒あたりの移動速度
    float moveSpeedSecond = 32.0f;
    // 1秒あたりの回転速度
    float rotateSpeedSecond = 6.0f;

    // 弾の発射間隔タイマー(秒)
    float shootIntervalTimer_ = 0.5f;
    // シェイク用のアニメーションタイマー(秒)
    float shakeAnimationTimer_ = 0.0f;
    // シェイクする時間(秒)
    float shakeAnimationDuration_ = 0.5f;

    // 生存フラグ
    bool isAlive_ = true;
};