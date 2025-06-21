#pragma once
#include "Objects.h"
#include "Math/MathObjects/AABB.h"

// 前方宣言
class MapChipField;
class Enemy;

class Player {
public:
    // キャラクターの当たり判定サイズ
    static inline const float kWidth = 1.8f;
    static inline const float kHeight = 1.8f;

    enum class LRDirection {
        kRight,
        kLeft,
    };

    enum Corner {
        kRightBottom,	// 右下
        kLeftBottom,	// 左下
        kRightTop,		// 右上
        kLeftTop,       // 左上

        kNumCorner		// 要素数
    };

    enum class Behaivior {
        kUnknown,	// 未定義
        kRoot,		// 通常行動
        kAttack,	// 攻撃行動
    };

    enum class AttackPhase {
        kStore,	// 溜め
        kRush,  // 突進
        kAfter, // 余韻
    };

    struct CollisionMapInfo {
        bool isHitUp = false;
        bool isHitGround = false;
        bool isHitWall = false;
        KashipanEngine::Vector3 velocity = { 0.0f, 0.0f, 0.0f };
    };

    /// @brief 初期化
    /// @param model モデル
    /// @param modelAttack 攻撃用モデル
    /// @param camera カメラ
    /// @param debugCamera デバッグカメラ
    /// @param position 初期位置
    void Initialize(KashipanEngine::Model *model, KashipanEngine::Model *modelAttack, const KashipanEngine::Vector3 &position);

    // 更新
    void Update();

    // 描画
    void Draw();

    // 座標の設定
    void SetPosition(const KashipanEngine::Vector3 &position) { worldTransform_->translate_ = position; }

    /// @brief マップチップフィールドの設定
    /// @param mapChipField マップチップフィールド
    void SetMapChipField(MapChipField *mapChipField) { mapChipField_ = mapChipField; }

    // 衝突応答
    void OnCollision(const Enemy *enemy);

    // ワールドトランスフォームの取得
    KashipanEngine::WorldTransform &GetWorldTransform() { return *worldTransform_; }

    // 速度の取得
    const KashipanEngine::Vector3 &GetVelocity() const { return velocity_; }

    // 座標の取得
    const KashipanEngine::Vector3 GetPosition();

    // AABBの取得
    KashipanEngine::Math::AABB GetAABB();

    // デスフラグの取得
    bool IsDead() const { return isDead_; }

private:
    // 通常行動更新
    void BehaiviorRootUpdate();
    // 移動処理
    void Move();
    // 当たり判定処理
    void CollisionCheck();
    void CollisionCheckUp(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckDown(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckLeft(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckRight(CollisionMapInfo &collisionMapInfo);
    // 判定結果を反映して移動させる
    void ApplyCollisionResult(const CollisionMapInfo &collisionMapInfo);
    // 天井に接している場合の処理
    void OnHitCeiling(const CollisionMapInfo &collisionMapInfo);
    // 壁に接している場合の処理
    void OnHitWall(const CollisionMapInfo &collisionMapInfo);
    // 接地状態の切り替え
    void SwitchOnGroundState(CollisionMapInfo &collisionMapInfo);
    // 旋回処理
    void Turn();
    // 攻撃行動処理
    void BehaiviorAttackUpdate();

    // 振る舞い状態の切り替え
    void SwitchBehaivior();

    // 通常行動初期化
    void BehaiviorRootInitialize();
    // 攻撃行動初期化
    void BehaiviorAttackInitialize();

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    KashipanEngine::Model *model_ = nullptr;
    // 攻撃用モデル
    KashipanEngine::Model *modelAttack_ = nullptr;
    // マップチップフィールド
    MapChipField *mapChipField_ = nullptr;

    // プレイヤーの移動速度
    KashipanEngine::Vector3 velocity_ = {};
    // プレイヤーの移動方向
    LRDirection lrDirection_ = LRDirection::kRight;
    // 旋回開始時の角度
    float turnFirstRotationY_ = 0.0f;
    // 旋回タイマー
    float turnTimer_ = 0.0f;
    // 接地状態フラグ
    bool onGround_ = true;

    // デスフラグ
    bool isDead_ = false;

    // 振るまい
    Behaivior behaivior_ = Behaivior::kRoot;
    // 次の振るまい
    Behaivior behaiviorRequest_ = Behaivior::kUnknown;

    // 攻撃ギミックの経過時間カウンター
    uint32_t attackParameter_ = 0;

    // 現在の攻撃フェーズ
    AttackPhase attackPhase_ = AttackPhase::kStore;
};
