#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Sphere : public Object {
    /// @brief 必ず分割数を設定してもらいたいのでデフォルトコンストラクタは削除
    Sphere() = delete;
    /// @brief 球体のコンストラクタ
    /// @param subdivision 分割数
    Sphere(const int subdivision) :
        kSubdivision(subdivision),
        kVertexCount(subdivision * subdivision * 4),
        kIndexCount(subdivision * subdivision * 6)
    {
        mesh = PrimitiveDrawer::CreateMesh(kVertexCount, kIndexCount);
        const float pi = 3.14159265358979323846f;
        // 経度分割1つ分の角度
        const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
        // 緯度分割1つ分の角度
        const float kLatEvery = pi / static_cast<float>(kSubdivision);
        // 緯度の方向に分割 -π/2 ～ π/2
        for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {
            const float lat = -pi / 2.0f + (kLatEvery * static_cast<float>(latIndex));
            // 経度の方向に分割 0 ～ 2π
            for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {
                const float lon = static_cast<float>(lonIndex) * kLonEvery;

                // インデックスを計算
                const uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;
                // 頂点位置を計算
                const uint32_t vertexIndex = (latIndex * kSubdivision + lonIndex) * 4;

                //--------- インデックスを設定 ---------//

                mesh->indexBufferMap[startIndex + 0] = vertexIndex + 0;
                mesh->indexBufferMap[startIndex + 1] = vertexIndex + 1;
                mesh->indexBufferMap[startIndex + 2] = vertexIndex + 2;
                mesh->indexBufferMap[startIndex + 3] = vertexIndex + 1;
                mesh->indexBufferMap[startIndex + 4] = vertexIndex + 3;
                mesh->indexBufferMap[startIndex + 5] = vertexIndex + 2;

                //--------- 頂点データを設定 ---------//

                // 左下
                mesh->vertexBufferMap[vertexIndex + 0].position = {
                    std::cosf(lat) * std::cosf(lon),
                    std::sinf(lat),
                    std::cosf(lat) * std::sinf(lon),
                    1.0f
                };
                mesh->vertexBufferMap[vertexIndex + 0].texCoord = {
                    static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
                    1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision)
                };
                // 左上
                mesh->vertexBufferMap[vertexIndex + 1].position = {
                    std::cosf(lat + kLatEvery) * std::cosf(lon),
                    std::sinf(lat + kLatEvery),
                    std::cosf(lat + kLatEvery) * std::sinf(lon),
                    1.0f
                };
                mesh->vertexBufferMap[vertexIndex + 1].texCoord = {
                    static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
                    1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision)
                };
                // 右下
                mesh->vertexBufferMap[vertexIndex + 2].position = {
                    std::cosf(lat) * std::cosf(lon + kLonEvery),
                    std::sinf(lat),
                    std::cosf(lat) * std::sinf(lon + kLonEvery),
                    1.0f
                };
                mesh->vertexBufferMap[vertexIndex + 2].texCoord = {
                    static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
                    1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision)
                };
                // 右上
                mesh->vertexBufferMap[vertexIndex + 3].position = {
                    std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
                    std::sinf(lat + kLatEvery),
                    std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
                    1.0f
                };
                mesh->vertexBufferMap[vertexIndex + 3].texCoord = {
                    static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
                    1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision)
                };
            }
        }
    }

    /// @brief 半径
    float radius;

    /// @brief 描画時の分割数
    const uint32_t kSubdivision;
    /// @brief 頂点数
    const uint32_t kVertexCount;
    /// @brief インデックス数
    const uint32_t kIndexCount;
};

} // namespace KashipanEngine