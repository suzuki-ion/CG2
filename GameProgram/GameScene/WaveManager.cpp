#include "WaveManager.h"
#include "Enemies/EnemyNormal.h"
#include "Enemies/EnemyStopMidway.h"

#include <KashipanEngine.h>
#include <Common/JsoncLoader.h>
#include <Common/Random.h>

using namespace KashipanEngine;

void WaveManager::Update() {
    if (!isWaveInProgress_) {
        return;
    }

    if (spawnTime_ > 0.0f) {
        spawnTime_ -= Engine::GetDeltaTime();
        if (spawnTime_ <= 0.0f) {
            SpawnEnemy();
            currentSpawnIndex_++;
        }
    }

    while (spawnTime_ <= 0.0f) {
        if (currentSpawnIndex_ >= static_cast<int>(waves_[currentWaveIndex_].size())) {
            // ウェーブ終了
            isWaveInProgress_ = false;
            isWaveCleared_ = true;
            break;
        }
        spawnTime_ = waves_[currentWaveIndex_][currentSpawnIndex_].spawnTime;
        if (spawnTime_ > 0.0f) {
            break;
        }
        SpawnEnemy();
        currentSpawnIndex_++;
    }
}

void WaveManager::LoadFromJson(const char *filePath) {
    for (auto &wave : waves_) {
        wave.clear();
    }
    waves_.clear();
    Json waveDataJson = LoadJsonc(filePath);
    for (const auto &waveFilePath : waveDataJson["Waves"]) {
        Json waveJson = LoadJsonc(waveFilePath.get<std::string>().c_str());
        LoadWave(waveJson);
    }
}

void WaveManager::LoadWave(const nlohmann::json &waveJson) {
    waves_.emplace_back();
    for (const auto &enemyJson : waveJson["Enemies"]) {
        EnemySpawnInfo info;
        info.type = kEnemyTypeMap.at(enemyJson["Type"].get<std::string>());
        info.bulletType = kBulletTypeMap.at(enemyJson["BulletType"].get<std::string>());
        info.spawnPosition = {
            enemyJson["SpawnPosition"]["x"].get<float>(),
            enemyJson["SpawnPosition"]["y"].get<float>(),
            0.0f // Z座標は固定なので0.0fに設定
        };
        info.level = enemyJson["Level"].get<int>();
        info.spawnTime = enemyJson["SpawnTime"].get<float>();
        info.isRandomSpawnPosition = enemyJson["IsRandomSpawnPosition"].get<bool>();
        waves_.back().emplace_back(info);
    }
}

void WaveManager::SpawnEnemy() {
    CameraPerspective cameraPerspective = camera_->GetCameraPerspective();
    const float height = 2.0f * 16.0f * std::tan(cameraPerspective.fovY * 0.5f) * 0.4f;
    const float width = height * cameraPerspective.aspectRatio;

    std::unique_ptr<BaseEntity> enemy = nullptr;
    EnemySpawnInfo info = waves_[currentWaveIndex_][currentSpawnIndex_];
    Vector3 spawnPosition;
    if (info.isRandomSpawnPosition) {
        spawnPosition = {
            GetRandomFloat(-width, width),
            GetRandomFloat(-height, height),
            128.0f
        };
    } else {
        spawnPosition = info.spawnPosition;
        spawnPosition.z = 128.0f;
    }

    switch (info.type) {
        case EnemyType::None:
            break;
        case EnemyType::Normal:
            enemy = std::make_unique<EnemyNormal>(
                spawnPosition,
                model_,
                bulletModel_,
                camera_
            );
            break;
        case EnemyType::StopMidway:
            enemy = std::make_unique<EnemyStopMidway>(
                spawnPosition,
                model_,
                bulletModel_,
                camera_
            );
            break;
        case EnemyType::Boss:
            break;
        default:
            break;
    }

    if (!enemy) {
        return;
    }
    enemy->SetShootBulletType(info.bulletType);
    enemy->SetLevel(info.level);
    spawnEnemyCallback_(std::move(enemy));
}
