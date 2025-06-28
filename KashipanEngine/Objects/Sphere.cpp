#include "Sphere.h"
#include "Math/Collider.h"
#include <cmath>
#define M_PI (4.0f * std::atanf(1.0f))

namespace KashipanEngine {

Sphere::Sphere(const int subdivision) :
    kSubdivision_(subdivision),
    kVertexCount_(subdivision *subdivision * 4),
    kIndexCount_(subdivision *subdivision * 6) {
    isUseCamera_ = true;
    Create(kVertexCount_, kIndexCount_);
    const float pi = 3.14159265358979323846f;
    // 経度分割1つ分の角度
    const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision_);
    // 緯度分割1つ分の角度
    const float kLatEvery = pi / static_cast<float>(kSubdivision_);
    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision_; latIndex++) {
        const float lat = -pi / 2.0f + (kLatEvery * static_cast<float>(latIndex));
        // 経度の方向に分割 0 ～ 2π
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; lonIndex++) {
            const float lon = static_cast<float>(lonIndex) * kLonEvery;

            // インデックスを計算
            const uint32_t startIndex = (latIndex * kSubdivision_ + lonIndex) * 6;
            // 頂点位置を計算
            const uint32_t vertexIndex = (latIndex * kSubdivision_ + lonIndex) * 4;

            //--------- インデックスを設定 ---------//

            mesh_->indexBufferMap[startIndex + 0] = vertexIndex + 0;
            mesh_->indexBufferMap[startIndex + 1] = vertexIndex + 1;
            mesh_->indexBufferMap[startIndex + 2] = vertexIndex + 2;
            mesh_->indexBufferMap[startIndex + 3] = vertexIndex + 1;
            mesh_->indexBufferMap[startIndex + 4] = vertexIndex + 3;
            mesh_->indexBufferMap[startIndex + 5] = vertexIndex + 2;

            //--------- 頂点データを設定 ---------//

            // 左下
            mesh_->vertexBufferMap[vertexIndex + 0].position = {
                std::cosf(lat) * std::cosf(lon),
                std::sinf(lat),
                std::cosf(lat) * std::sinf(lon),
                1.0f
            };
            mesh_->vertexBufferMap[vertexIndex + 0].texCoord = {
                static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_),
                1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_)
            };
            // 左上
            mesh_->vertexBufferMap[vertexIndex + 1].position = {
                std::cosf(lat + kLatEvery) * std::cosf(lon),
                std::sinf(lat + kLatEvery),
                std::cosf(lat + kLatEvery) * std::sinf(lon),
                1.0f
            };
            mesh_->vertexBufferMap[vertexIndex + 1].texCoord = {
                static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_),
                1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision_)
            };
            // 右下
            mesh_->vertexBufferMap[vertexIndex + 2].position = {
                std::cosf(lat) * std::cosf(lon + kLonEvery),
                std::sinf(lat),
                std::cosf(lat) * std::sinf(lon + kLonEvery),
                1.0f
            };
            mesh_->vertexBufferMap[vertexIndex + 2].texCoord = {
                static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision_),
                1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_)
            };
            // 右上
            mesh_->vertexBufferMap[vertexIndex + 3].position = {
                std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
                std::sinf(lat + kLatEvery),
                std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
                1.0f
            };
            mesh_->vertexBufferMap[vertexIndex + 3].texCoord = {
                static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision_),
                1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision_)
            };
        }
    }
    CalcNormal();
}

void Sphere::Draw() {
    // 前までの法線タイプが異なる場合は再計算
    if (preNormalType_ != normalType_) {
        CalcNormal();
        preNormalType_ = normalType_;
    }

    // 描画共通処理を呼び出す
    DrawCommon();
}

void Sphere::Draw(WorldTransform &worldTransform) {
    // 前までの法線タイプが異なる場合は再計算
    if (preNormalType_ != normalType_) {
        CalcNormal();
        preNormalType_ = normalType_;
    }

    // 描画共通処理を呼び出す
    DrawCommon(worldTransform);
}

void Sphere::CalcNormal() {
    Vector3 position[3] = {};
    // 緯度の方向に分割
    for (uint32_t latIndex = 0; latIndex < kSubdivision_; latIndex++) {
        // 経度の方向に分割
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; lonIndex++) {
            // 頂点位置を計算
            const uint32_t vertexIndex = (latIndex * kSubdivision_ + lonIndex) * 4;

            //--------- 法線を設定 ---------//

            if (material_.enableLighting == false) {
                for (int i = 0; i < 4; i++) {
                    mesh_->vertexBufferMap[vertexIndex + i].normal = { 0.0f, 0.0f, -1.0f };
                }
                continue;
            }
            if (normalType_ == kNormalTypeVertex) {
                mesh_->vertexBufferMap[vertexIndex + 0].normal =
                    Vector3(mesh_->vertexBufferMap[vertexIndex + 0].position);
                mesh_->vertexBufferMap[vertexIndex + 1].normal =
                    Vector3(mesh_->vertexBufferMap[vertexIndex + 1].position);
                mesh_->vertexBufferMap[vertexIndex + 2].normal =
                    Vector3(mesh_->vertexBufferMap[vertexIndex + 2].position);
                mesh_->vertexBufferMap[vertexIndex + 3].normal =
                    Vector3(mesh_->vertexBufferMap[vertexIndex + 3].position);

            } else if (normalType_ == kNormalTypeFace) {
                position[0] = Vector3(mesh_->vertexBufferMap[vertexIndex + 0].position);
                position[1] = Vector3(mesh_->vertexBufferMap[vertexIndex + 1].position);
                position[2] = Vector3(mesh_->vertexBufferMap[vertexIndex + 2].position);
                for (int i = 0; i < 4; i++) {
                    mesh_->vertexBufferMap[vertexIndex + i].normal =
                        (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
                }
            }
        }
    }
    // 面法線の場合、真下の面の法線だけ別で計算する
    if (normalType_ == kNormalTypeFace) {
        for (uint32_t i = 0; i < kSubdivision_; i++) {
            const uint32_t vertexIndex = i * 4;
            position[0] = Vector3(mesh_->vertexBufferMap[vertexIndex + 1].position);
            position[1] = Vector3(mesh_->vertexBufferMap[vertexIndex + 3].position);
            position[2] = Vector3(mesh_->vertexBufferMap[vertexIndex + 2].position);
            for (int j = 0; j < 4; j++) {
                mesh_->vertexBufferMap[vertexIndex + j].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }
}

} // namespace KashipanEngine