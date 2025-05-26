#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Plane : public Object {
    /// @brief 必ずカメラを設定してもらいたいのでデフォルトコンストラクタは削除
    Plane() = delete;
    /// @brief ビルボードのコンストラクタ
    /// @param camera カメラ
    Plane(Camera *camera) {
        mesh = PrimitiveDrawer::CreateMesh(4, 6);
        this->camera = camera;
        mesh->indexBufferMap[0] = 0;
        mesh->indexBufferMap[1] = 1;
        mesh->indexBufferMap[2] = 2;
        mesh->indexBufferMap[3] = 1;
        mesh->indexBufferMap[4] = 3;
        mesh->indexBufferMap[5] = 2;
    }
};

} // namespace KashipanEngine