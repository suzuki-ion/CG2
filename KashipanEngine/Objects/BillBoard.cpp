#include "BillBoard.h"
#include "Math/Camera.h"

namespace KashipanEngine {

BillBoard::BillBoard(Vector3 *cameraRotete) {
    Create(4, 6);
    isUseCamera_ = true;
    mesh_->indexBufferMap[0] = 0;
    mesh_->indexBufferMap[1] = 1;
    mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 1;
    mesh_->indexBufferMap[4] = 3;
    mesh_->indexBufferMap[5] = 2;
    cameraRotate_ = cameraRotete;
}

void BillBoard::Draw() {
    // 向きをカメラの向きに合わせる
    if (isUseX_) {
        transform_.rotate.x = cameraRotate_->x;
    } else {
        transform_.rotate.x = 0.0f;
    }
    if (isUseY_) {
        transform_.rotate.y = cameraRotate_->y;
    } else {
        transform_.rotate.y = 0.0f;
    }
    if (isUseZ_) {
        transform_.rotate.z = cameraRotate_->z;
    } else {
        transform_.rotate.z = 0.0f;
    }

    if (material_.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3] = {};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

void BillBoard::Draw(WorldTransform &worldTransform) {
    // 向きをカメラの向きに合わせる
    if (isUseX_) {
        worldTransform.rotate_.x = cameraRotate_->x;
    } else {
        worldTransform.rotate_.x = 0.0f;
    }
    if (isUseY_) {
        worldTransform.rotate_.y = cameraRotate_->y;
    } else {
        worldTransform.rotate_.y = 0.0f;
    }
    if (isUseZ_) {
        worldTransform.rotate_.z = cameraRotate_->z;
    } else {
        worldTransform.rotate_.z = 0.0f;
    }

    if (material_.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            mesh_->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (normalType_ == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    Vector3(mesh_->vertexBufferMap[i].position);
            }
        } else if (normalType_ == kNormalTypeFace) {
            Vector3 position[3] = {};
            position[0] = Vector3(mesh_->vertexBufferMap[0].position);
            position[1] = Vector3(mesh_->vertexBufferMap[1].position);
            position[2] = Vector3(mesh_->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                mesh_->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // 描画共通処理を呼び出す
    DrawCommon(worldTransform);
}

} // namespace KashipanEngine