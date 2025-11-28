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

// GPU 用の Transform / Material / Particle レイアウト(シェーダと一致させる)
struct GpuTransform {
    float scale[3]; float _pad0;
    float rotate[3]; float _pad1;
    float translate[3]; float _pad2;
};

struct GpuMaterial {
    float color[4];
    int32_t lightingType;
    int32_t _pad0[3];
    float uvTransform[16];
    float diffuseColor[4];
    float specularColor[4];
    float emissiveColor[4];
};

struct GpuParticle {
    uint32_t isActive;
    uint32_t isAlive;
    int32_t textureIndex;
    int32_t _pad0;
    GpuTransform transform;
    GpuMaterial material;
    float velocity[3];
    float _padV;
};

struct Particle {
    bool isActive = true;        // 更新対象かどうか
    bool isAlive = true;         // 生存しているかどうか
    uint32_t textureIndex = 0;   // 使用テクスチャ
    Transform transform;         // 個別トランスフォーム
    Material material;           // 個別マテリアル(未対応)
    Vector3 velocity = {0.0f, 0.0f, 0.0f};
};

struct GpuParticleCB {
    float deltaTime;
    float pad0[3];
    Vector4 spawnPos;
    uint32_t maxCount;
    float pad1;
    uint32_t instanceCount;
    float noiseTime;
    Matrix4x4 viewProjection;

    float noiseScale;
    float noiseAmplitudeBase;
    float timeSpeedX;
    float timeSpeedY;
    float timeSpeedZ;
    float curlStrength;
    float damping;
    float eps;
    Vector3 boxHalfExtent;
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
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetMatricesSrvGPU() const { return useCompute_ ? matricesSrvGPUDefault_ : matricesSrvGPUUpload_; }
    /// @brief テクスチャインデックス取得
    [[nodiscard]] int GetTextureIndex() const { return static_cast<int>(textureIndex_); }

    /// @brief パーティクル定数バッファ取得
    [[nodiscard]] GpuParticleCB *GetGpuParticleCB() { return &particleCB_; }

    void UpdateMatrices(const Matrix4x4 &viewProjection);
    void Draw() override;

    // コンピュートシェーダー使用有効化/無効化
    void SetUseCompute(bool use) { useCompute_ = use; }
    bool IsUseCompute() const { return useCompute_; }

private:
    void InitializeResources();
    void DispatchCompute(float deltaTime, const Matrix4x4 &viewProjection); // コンピュート更新

    DirectXCommon *dxCommon_ = nullptr;
    uint32_t maxInstances_ = 0;
    uint32_t textureIndex_ = 0;

    // パーティクル配列
    std::vector<std::unique_ptr<Particle>> particles_;

    // アクティブインスタンス数
    uint32_t activeInstanceCount_ = 0;

    // 行列配列用（CPU更新用: Upload + SRV）
    Microsoft::WRL::ComPtr<ID3D12Resource> matricesResource_;
    TransformationMatrix *matricesMap_ = nullptr;
    D3D12_GPU_DESCRIPTOR_HANDLE matricesSrvGPUUpload_{};

    // 行列配列用（GPU更新用: Default + UAV/SRV）
    Microsoft::WRL::ComPtr<ID3D12Resource> matricesResourceDefault_;
    D3D12_GPU_DESCRIPTOR_HANDLE matricesSrvGPUDefault_{};

    // パーティクル配列（GPU更新用）
    Microsoft::WRL::ComPtr<ID3D12Resource> particlesUpload_;
    Microsoft::WRL::ComPtr<ID3D12Resource> particlesResourceDefault_;
    // UAVテーブル先頭
    D3D12_GPU_DESCRIPTOR_HANDLE uavTableStartGPU_{}; // u0: matricesOut, u1: particles
    // CBV（GPU更新用）
    Microsoft::WRL::ComPtr<ID3D12Resource> computeCB_;
    void *computeCBMap_ = nullptr;
    D3D12_GPU_DESCRIPTOR_HANDLE computeCBVGPU_{};

    bool positionsInitialized_ = false;
    bool inUAVState_ = true;

    // パーティクル定数バッファ
    GpuParticleCB particleCB_{};

    // 生成(発生)位置
    Vector3 spawnPosition_ = { 0.0f, 0.0f, 0.0f };

    bool useCompute_ = false; // コンピュートシェーダーで更新するか
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