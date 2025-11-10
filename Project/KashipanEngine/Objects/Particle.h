#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <d3d12.h>

#include "Objects/Object.h"
#include "Common/TransformationMatrix.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace KashipanEngine {

// 前方宣言
class Renderer;
class DirectXCommon;

struct Particle {
    bool isActive = true;        // 更新対象かどうか
    bool isAlive = true;         // 生存しているかどうか
    uint32_t textureIndex = 0;   // 使用テクスチャ
    Transform transform;         // 個別トランスフォーム
    Material material;           // 個別マテリアル(未対応)
};

/// @brief パーティクルグループクラス
class ParticleGroup : public Object {
public:
    ParticleGroup() = delete;
    /// @brief コンストラクタ
    /// @param dxCommon DirectX共通
    /// @param maxInstances 最大インスタンス数(バッファ確保数)
    /// @param textureIndex 使用するテクスチャインデックス
    ParticleGroup(DirectXCommon *dxCommon, uint32_t maxInstances, uint32_t textureIndex);
    ~ParticleGroup() = default;

    /// @brief パーティクル生成(グループの発生位置から生成)
    /// @return 生成したパーティクルへのポインタ
    Particle *SpawnParticle();

    /// @brief パーティクル生成(指定位置に生成)
    /// @param position 生成位置
    /// @return 生成したパーティクルへのポインタ
    Particle *SpawnParticle(const Vector3 &position);

    /// @brief パーティクル削除(非活性化)
    void KillParticle(size_t index);

    /// @brief 全パーティクル取得
    const std::vector<std::unique_ptr<Particle>> &GetParticles() const { return particles_; }

    /// @brief アクティブなインスタンス数取得
    [[nodiscard]] uint32_t GetInstanceCount() const { return activeInstanceCount_; }
    /// @brief インデックス数取得
    [[nodiscard]] UINT GetIndexCount() const { return indexCount_; }
    /// @brief メッシュ取得
    [[nodiscard]] Mesh<VertexData> *GetMesh() const { return mesh_.get(); }
    /// @brief マテリアルリソース取得
    [[nodiscard]] ID3D12Resource *GetMaterialResource() const { return materialResource_.Get(); }
    /// @brief TransformationMatrices 用 SRV GPUハンドル取得
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetMatricesSrvGPU() const { return matricesSrvGPU_; }
    /// @brief テクスチャインデックス取得
    [[nodiscard]] int GetTextureIndex() const { return static_cast<int>(textureIndex_); }

    /// @brief 生成(発生)位置を設定
    void SetSpawnPosition(const Vector3 &position) { spawnPosition_ = position; }
    /// @brief 生成(発生)位置を取得
    [[nodiscard]] const Vector3 &GetSpawnPosition() const { return spawnPosition_; }

    void UpdateMatrices(const Matrix4x4 &viewProjection);
    void Draw() override;

private:
    void InitializeResources();

    DirectXCommon *dxCommon_ = nullptr;
    uint32_t maxInstances_ = 0;
    uint32_t textureIndex_ = 0;

    // パーティクル配列
    std::vector<std::unique_ptr<Particle>> particles_;

    // アクティブインスタンス数
    uint32_t activeInstanceCount_ = 0;

    // 行列配列用
    Microsoft::WRL::ComPtr<ID3D12Resource> matricesResource_;
    TransformationMatrix *matricesMap_ = nullptr;
    D3D12_GPU_DESCRIPTOR_HANDLE matricesSrvGPU_{};

    // 生成(発生)位置
    Vector3 spawnPosition_ = { 0.0f, 0.0f, 0.0f };
};

/// @brief パーティクルマネージャ
class ParticleManager {
public:
    /// @brief パーティクルの全削除
    static void ClearAllParticleGroups();

    /// @brief パーティクルグループ作成
    static ParticleGroup *CreateParticleGroup(const std::string &name, DirectXCommon *dxCommon, uint32_t maxInstances, uint32_t textureIndex);
    /// @brief 取得
    static ParticleGroup *GetParticleGroup(const std::string &name);
};

} // namespace KashipanEngine