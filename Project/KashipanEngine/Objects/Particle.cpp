#include "Particle.h"
#include "KashipanEngine.h"
#include "Base/DirectXCommon.h"
#include "3d/PrimitiveDrawer.h"
#include "Common/Descriptors/SRV.h"
#include "Base/Renderer.h"
#include "Common/VertexData.h"
#include "Common/Logs.h"
#include "Base/Texture.h"
#include <cstring>

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
    materialMap_->lightingType = 1;
    materialMap_->uvTransform = Matrix4x4::Identity();
    materialMap_->diffuseColor = {1,1,1,1};
    materialMap_->specularColor = {1,1,1,1};
    materialMap_->emissiveColor = {1,1,1,1};

    const UINT64 matricesBytes = sizeof(TransformationMatrix) * maxInstances_;
    matricesResource_ = PrimitiveDrawer::CreateBufferResources(matricesBytes);
    matricesResource_->Map(0, nullptr, reinterpret_cast<void **>(&matricesMap_));

    // アップロード用SRV（CPUで更新される）行列
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = maxInstances_;
        srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        auto cpuHandle = SRV::GetCPUDescriptorHandle();
        matricesSrvGPUUpload_ = SRV::GetGPUDescriptorHandle();
        dxCommon_->GetDevice()->CreateShaderResourceView(matricesResource_.Get(), &srvDesc, cpuHandle);
    }

    // GPUで更新される行列とパーティクル用のデフォルトバッファ
    {
        D3D12_HEAP_PROPERTIES heapProps{}; heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC bufDesc{};
        bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufDesc.Width = matricesBytes;
        bufDesc.Height = 1; bufDesc.DepthOrArraySize = 1; bufDesc.MipLevels = 1; bufDesc.SampleDesc.Count = 1; bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        dxCommon_->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&matricesResourceDefault_));

        // デフォルト行列用SRV
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = maxInstances_;
            srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
            auto cpu = SRV::GetCPUDescriptorHandle();
            matricesSrvGPUDefault_ = SRV::GetGPUDescriptorHandle();
            dxCommon_->GetDevice()->CreateShaderResourceView(matricesResourceDefault_.Get(), &srvDesc, cpu);
        }

        // UAVを作成：行列とパーティクル、UAVテーブル開始位置を設定
        {
            auto uav0 = SRV::GetCPUDescriptorHandle();
            uavTableStartGPU_ = SRV::GetGPUDescriptorHandle();
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER; uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.Buffer.NumElements = maxInstances_; uavDesc.Buffer.FirstElement = 0; uavDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
            dxCommon_->GetDevice()->CreateUnorderedAccessView(matricesResourceDefault_.Get(), nullptr, &uavDesc, uav0);

            // パーティクル用バッファ
            const UINT64 particleBytes = sizeof(GpuParticle) * maxInstances_;
            D3D12_RESOURCE_DESC partDesc = bufDesc; partDesc.Width = particleBytes;
            dxCommon_->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &partDesc,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&particlesResourceDefault_));

            auto uav1 = SRV::GetCPUDescriptorHandle();
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavPart{}; uavPart.ViewDimension = D3D12_UAV_DIMENSION_BUFFER; uavPart.Format = DXGI_FORMAT_UNKNOWN;
            uavPart.Buffer.NumElements = maxInstances_; uavPart.Buffer.FirstElement = 0; uavPart.Buffer.StructureByteStride = sizeof(GpuParticle);
            dxCommon_->GetDevice()->CreateUnorderedAccessView(particlesResourceDefault_.Get(), nullptr, &uavPart, uav1);
        }

        // 初期パーティクルデータ用のアップロードバッファ
        particlesUpload_ = PrimitiveDrawer::CreateBufferResources(sizeof(GpuParticle) * maxInstances_);
        computeCB_ = PrimitiveDrawer::CreateBufferResources(1024);
        computeCB_->Map(0, nullptr, reinterpret_cast<void **>(&computeCBMap_));

        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv{};
            cbv.BufferLocation = computeCB_->GetGPUVirtualAddress();
            cbv.SizeInBytes = 1024;
            auto cbCpu = SRV::GetCPUDescriptorHandle();
            computeCBVGPU_ = SRV::GetGPUDescriptorHandle();
            dxCommon_->GetDevice()->CreateConstantBufferView(&cbv, cbCpu);
        }

        particleCB_.deltaTime = 0.0f;
        particleCB_.spawnPos = Vector4(spawnPosition_.x, spawnPosition_.y, spawnPosition_.z, 1.0f);
        particleCB_.maxCount = maxInstances_;
        particleCB_.instanceCount = 0;
        particleCB_.noiseTime = 0.0f;
        particleCB_.viewProjection = Matrix4x4::Identity();

        particleCB_.noiseScale = 0.6f;
        particleCB_.noiseAmplitudeBase = 1.2f;
        particleCB_.timeSpeedX = 0.5f;
        particleCB_.timeSpeedY = 0.6f;
        particleCB_.timeSpeedZ = 0.7f;
        particleCB_.curlStrength = 4.0f;
        particleCB_.damping = 0.95f;
        particleCB_.eps = 0.01f;
        particleCB_.boxHalfExtent = Vector3(128.0f, 128.0f, 128.0f);

        std::memcpy(computeCBMap_, &particleCB_, sizeof(particleCB_));
    }
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
                p->velocity = {0.0f, 0.0f, 0.0f};
                return p.get();
            }
        }
        return nullptr;
    }
    auto p = std::make_unique<Particle>();
    p->transform.translate = position;
    p->velocity = {0.0f, 0.0f, 0.0f};
    particles_.push_back(std::move(p));
    return particles_.back().get();
}

void ParticleGroup::KillParticle(size_t index) {
    if (index < particles_.size()) {
        particles_[index]->isAlive = false;
        particles_[index]->isActive = false;
    }
}

void ParticleGroup::DispatchCompute(float deltaTime, const Matrix4x4 &viewProjection) {
    auto *cmd = dxCommon_->GetCommandList();

    {
        ID3D12DescriptorHeap *heaps[] = { SRV::GetDescriptorHeap() };
        cmd->SetDescriptorHeaps(1, heaps);
    }

    if (!inUAVState_) {
        D3D12_RESOURCE_BARRIER toUAV{};
        toUAV.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        toUAV.Transition.pResource = matricesResourceDefault_.Get();
        toUAV.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE; // 以前は描画用のSRV状態
        toUAV.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        toUAV.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        dxCommon_->SetBarrier(toUAV);
        inUAVState_ = true;
    }

    // 初回のみparticlesを初期化（Spawn済みの粒子のみ）
    if (!positionsInitialized_) {
        GpuParticle *map = nullptr;
        particlesUpload_->Map(0, nullptr, reinterpret_cast<void **>(&map));
        UINT count = 0;
        for (auto &pp : particles_) {
            if (!pp->isActive || !pp->isAlive) { continue; }
            GpuParticle gp{};
            gp.isActive = pp->isActive ? 1u : 0u;
            gp.isAlive = pp->isAlive ? 1u : 0u;
            gp.textureIndex = static_cast<int32_t>(pp->textureIndex);
            gp._pad0 = 0;
            gp.transform.scale[0] = pp->transform.scale.x; gp.transform.scale[1] = pp->transform.scale.y; gp.transform.scale[2] = pp->transform.scale.z;
            gp.transform.rotate[0] = pp->transform.rotate.x; gp.transform.rotate[1] = pp->transform.rotate.y; gp.transform.rotate[2] = pp->transform.rotate.z;
            gp.transform.translate[0] = pp->transform.translate.x; gp.transform.translate[1] = pp->transform.translate.y; gp.transform.translate[2] = pp->transform.translate.z;
            gp.material.color[0] = pp->material.color.x; gp.material.color[1] = pp->material.color.y; gp.material.color[2] = pp->material.color.z; gp.material.color[3] = pp->material.color.w;
            gp.material.lightingType = pp->material.lightingType;
            // map CPU-side velocity to GPU particle struct
            gp.velocity[0] = pp->velocity.x; gp.velocity[1] = pp->velocity.y; gp.velocity[2] = pp->velocity.z;

            map[count++] = gp;
            if (count >= maxInstances_) { break; }
        }
        activeInstanceCount_ = count;

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = particlesResourceDefault_.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        dxCommon_->SetBarrier(barrier);

        cmd->CopyBufferRegion(particlesResourceDefault_.Get(), 0, particlesUpload_.Get(), 0, sizeof(GpuParticle) * activeInstanceCount_);

        std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
        dxCommon_->SetBarrier(barrier);

        positionsInitialized_ = true;
    }

    if (renderer_) { renderer_->SetPipeLine("Particle.Update"); }

    // UAVテーブルをバインド（u0: 行列、u1: パーティクル）
    cmd->SetComputeRootDescriptorTable(0, uavTableStartGPU_);

    static float noiseTime = 0.0f;
    noiseTime += deltaTime * 0.1f;
    particleCB_.deltaTime = deltaTime;
    particleCB_.maxCount = maxInstances_;
    particleCB_.instanceCount = activeInstanceCount_;
    particleCB_.noiseTime = noiseTime;
    particleCB_.viewProjection = viewProjection;
    particleCB_.spawnPos = Vector4(spawnPosition_.x, spawnPosition_.y, spawnPosition_.z, 1.0f);
    std::memcpy(computeCBMap_, &particleCB_, sizeof(particleCB_));

    cmd->SetComputeRootConstantBufferView(1, computeCB_->GetGPUVirtualAddress());

    UINT threads = 1024;
    UINT groups = (activeInstanceCount_ + threads - 1) / threads;
    if (groups == 0) { groups = 1; }
    cmd->Dispatch(groups, 1, 1);

    {
        D3D12_RESOURCE_BARRIER uavBarrier{};
        uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        uavBarrier.UAV.pResource = matricesResourceDefault_.Get();
        cmd->ResourceBarrier(1, &uavBarrier);
    }
    if (inUAVState_) {
        D3D12_RESOURCE_BARRIER toSRV{};
        toSRV.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        toSRV.Transition.pResource = matricesResourceDefault_.Get();
        toSRV.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        toSRV.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        toSRV.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        dxCommon_->SetBarrier(toSRV);
        inUAVState_ = false;
    }
}

void ParticleGroup::UpdateMatrices(const Matrix4x4 &viewProjection) {
    if (useCompute_) {
        float dt = Engine::GetDeltaTime();
        DispatchCompute(dt, viewProjection);
        /*if (activeInstanceCount_ == 0 && !particles_.empty()) {
            uint32_t count = 0;
            for (auto &pp : particles_) {
                if (!pp->isActive || !pp->isAlive) { continue; }
                ++count;
                if (count >= maxInstances_) { break; }
            }
            activeInstanceCount_ = count;
        }*/
        return;
    }
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
    if (it != sParticleGroups.end()) {
        return it->second.get();
    }
    return nullptr;
}

} // namespace KashipanEngine
