#include "Particle.h"
#include "Base/DirectXCommon.h"
#include "3d/PrimitiveDrawer.h"
#include "Common/Descriptors/SRV.h"
#include "Base/Renderer.h"
#include "Common/VertexData.h"
#include "Common/Logs.h"
#include "Base/Texture.h"

namespace KashipanEngine {

namespace {
std::unordered_map<std::string, std::unique_ptr<ParticleGroup>> sParticleGroups;
} // namespace

ParticleGroup::ParticleGroup(DirectXCommon *dxCommon, uint32_t maxInstances, uint32_t textureIndex)
    : dxCommon_(dxCommon), maxInstances_(maxInstances), textureIndex_(textureIndex) {
    assert(dxCommon_);
    InitializeResources();
}

void ParticleGroup::InitializeResources() {
    pipeLineName_ = "Particle.Solid.BlendNormal";
    useTextureIndex_ = static_cast<int>(textureIndex_);

    vertexCount_ = 4;
    indexCount_ = 6;
    mesh_ = PrimitiveDrawer::CreateMesh<VertexData>(vertexCount_, indexCount_, sizeof(VertexData));

    mesh_->vertexBufferMap[0].position = { -0.5f,  0.5f, 0.0f, 1.0f }; mesh_->vertexBufferMap[0].texCoord = { 0.0f, 0.0f }; mesh_->vertexBufferMap[0].normal = { 0.0f, 0.0f, -1.0f };
    mesh_->vertexBufferMap[1].position = {  0.5f,  0.5f, 0.0f, 1.0f }; mesh_->vertexBufferMap[1].texCoord = { 1.0f, 0.0f }; mesh_->vertexBufferMap[1].normal = { 0.0f, 0.0f, -1.0f };
    mesh_->vertexBufferMap[2].position = {  0.5f, -0.5f, 0.0f, 1.0f }; mesh_->vertexBufferMap[2].texCoord = { 1.0f, 1.0f }; mesh_->vertexBufferMap[2].normal = { 0.0f, 0.0f, -1.0f };
    mesh_->vertexBufferMap[3].position = { -0.5f, -0.5f, 0.0f, 1.0f }; mesh_->vertexBufferMap[3].texCoord = { 0.0f, 1.0f }; mesh_->vertexBufferMap[3].normal = { 0.0f, 0.0f, -1.0f };

    mesh_->indexBufferMap[0] = 0; mesh_->indexBufferMap[1] = 1; mesh_->indexBufferMap[2] = 2;
    mesh_->indexBufferMap[3] = 0; mesh_->indexBufferMap[4] = 2; mesh_->indexBufferMap[5] = 3;

    materialResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(Material));
    materialResource_->Map(0, nullptr, reinterpret_cast<void **>(&materialMap_));
    materialMap_->color = {1,1,1,1};
    materialMap_->lightingType = 0;
    materialMap_->uvTransform = Matrix4x4::Identity();
    materialMap_->diffuseColor = {1,1,1,1};
    materialMap_->specularColor = {1,1,1,1};
    materialMap_->emissiveColor = {1,1,1,1};

    const UINT64 bufferSize = sizeof(TransformationMatrix) * maxInstances_;
    matricesResource_ = PrimitiveDrawer::CreateBufferResources(bufferSize);
    matricesResource_->Map(0, nullptr, reinterpret_cast<void **>(&matricesMap_));

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = maxInstances_;
    srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    auto cpuHandle = SRV::GetCPUDescriptorHandle();
    matricesSrvGPU_ = SRV::GetGPUDescriptorHandle();
    dxCommon_->GetDevice()->CreateShaderResourceView(matricesResource_.Get(), &srvDesc, cpuHandle);
}

Particle *ParticleGroup::SpawnParticle() {
    // デフォルト生成位置を使用
    return SpawnParticle(spawnPosition_);
}

Particle *ParticleGroup::SpawnParticle(const Vector3 &position) {
    if (particles_.size() >= maxInstances_) {
        for (auto &p : particles_) {
            if (!p->isActive || !p->isAlive) {
                p->isActive = true;
                p->isAlive = true;
                p->transform.translate = position;
                return p.get();
            }
        }
        return nullptr;
    }
    auto p = std::make_unique<Particle>();
    p->transform.translate = position;
    particles_.push_back(std::move(p));
    return particles_.back().get();
}

void ParticleGroup::KillParticle(size_t index) {
    if (index < particles_.size()) {
        particles_[index]->isAlive = false;
        particles_[index]->isActive = false;
    }
}

void ParticleGroup::UpdateMatrices(const Matrix4x4 &viewProjection) {
    activeInstanceCount_ = 0;
    for (size_t i = 0; i < particles_.size(); ++i) {
        auto &p = particles_[i];
        if (!p->isActive || !p->isAlive) { continue; }
        Matrix4x4 world; world.MakeAffine(p->transform.scale, p->transform.rotate, p->transform.translate);
        matricesMap_[activeInstanceCount_].world = world;
        matricesMap_[activeInstanceCount_].wvp = world * viewProjection;
        matricesMap_[activeInstanceCount_].viewportInverse.MakeIdentity();
        activeInstanceCount_++;
        if (activeInstanceCount_ >= maxInstances_) { break; }
    }
}

void ParticleGroup::Draw() {
    if (!renderer_ || !isDraw_) { return; }
    renderer_->DrawParticles(this);
}

void ParticleManager::ClearAllParticleGroups() {
    sParticleGroups.clear();
}

ParticleGroup *ParticleManager::CreateParticleGroup(const std::string &name, DirectXCommon *dxCommon, uint32_t maxInstances, uint32_t textureIndex) {
    auto group = std::make_unique<ParticleGroup>(dxCommon, maxInstances, textureIndex);
    sParticleGroups[name] = std::move(group);
    return sParticleGroups[name].get();
}

ParticleGroup *ParticleManager::GetParticleGroup(const std::string &name) {
    auto it = sParticleGroups.find(name);
    if (it != sParticleGroups.end()) { return it->second.get(); }
    return nullptr;
}

} // namespace KashipanEngine
