#pragma once
#include "Object.h"

namespace KashipanEngine {

class BillBoard : public Object {
public:
    /// @brief 必ずカメラの回転角を設定してもらいたいのでデフォルトコンストラクタは削除
    BillBoard() = delete;
    /// @brief ビルボードのコンストラクタ
    /// @param cameraRotete カメラの回転角
    BillBoard(Vector3 *cameraRotete);

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param worldTransform ワールド変換データ
    void Draw(WorldTransform &worldTransform);

private:
    /// @brief カメラの回転角
    Vector3 *cameraRotate_ = nullptr;

    /// @brief カメラのX軸に追従させるかのフラグ
    bool isUseX_ = true;
    /// @brief カメラのY軸に追従させるかのフラグ
    bool isUseY_ = true;
    /// @brief カメラのZ軸に追従させるかのフラグ
    bool isUseZ_ = true;
};

} // namespace KashipanEngine