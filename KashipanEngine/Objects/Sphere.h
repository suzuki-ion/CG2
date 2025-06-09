#pragma once
#include "Object.h"

namespace KashipanEngine {

class Sphere : public Object {
public:
    /// @brief 必ず分割数を設定してもらいたいのでデフォルトコンストラクタは削除
    Sphere() = delete;
    /// @brief 球体のコンストラクタ
    /// @param subdivision 分割数
    Sphere(const int subdivision);

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

    /// @brief 球体の半径を取得
    /// @return 半径
    [[nodiscard]] float GetRadius() const {
        return radius_;
    }

    /// @brief 球体の半径を設定
    /// @param radius 半径
    void SetRadius(float radius) {
        radius_ = radius;
    }

private:
    /// @brief 半径
    float radius_ = 1.0f;

    /// @brief 描画時の分割数
    const uint32_t kSubdivision_;
    /// @brief 頂点数
    const uint32_t kVertexCount_;
    /// @brief インデックス数
    const uint32_t kIndexCount_;
};

} // namespace KashipanEngine