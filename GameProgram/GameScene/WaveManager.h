#pragma once
#include <json.hpp>
#include <Objects/Model.h>
#include <Math/Camera.h>
#include "BaseEntity.h"
#include "Enemies/EnemyType.h"
#include "Bullets/BaseBullet.h"

struct EnemySpawnInfo {
    EnemyType type;
    BulletType bulletType;
    KashipanEngine::Vector3 spawnPosition;
    int level; // 敵のレベル
    float spawnTime; // スポーンするまでの時間（秒）
    bool isRandomSpawnPosition; // trueならspawnPositionは無視され、ランダムな位置にスポーンする
};

class WaveManager {
public:
    WaveManager(
        const char *waveJsonFilePath,
        std::function<void(std::unique_ptr<BaseEntity> enemy)> spawnEnemyCallback,
        KashipanEngine::Model *model,
        KashipanEngine::Model *bulletModel,
        KashipanEngine::Camera *camera) {

        spawnEnemyCallback_ = spawnEnemyCallback;
        model_ = model;
        bulletModel_ = bulletModel;
        camera_ = camera;
        LoadFromJson(waveJsonFilePath);
    }
    ~WaveManager() = default;

    void NextWavePrepare() {
        if (!isWaveCleared_) {
            return;
        }
        currentWaveIndex_++;
        if (currentWaveIndex_ >= static_cast<int>(waves_.size())) {
            currentWaveIndex_--;
            return;
        }
        isWaveReady_ = true;
        isWaveInProgress_ = false;
        isWaveCleared_ = false;
        currentSpawnIndex_ = 0;
        spawnTime_ = 0.0f;
    }
    void StartWave() {
        if (!isWaveReady_) {
            return;
        }
        if (currentWaveIndex_ >= static_cast<int>(waves_.size())) {
            return;
        }
        isWaveInProgress_ = true;
        isWaveCleared_ = false;
        isWaveReady_ = false;
    }
    void ClearWave() {
        currentWaveIndex_ = 0;
        currentSpawnIndex_ = 0;
        spawnTime_ = 0.0f;
        isWaveInProgress_ = false;
        isWaveCleared_ = false;
        isWaveReady_ = true;
    }

    void Update();

    /// @brief Jsonファイルからウェーブ情報を読み込む
    /// @param filename Jsonファイルのパス
    void LoadFromJson(const char *filePath);

    int GetCurrentWaveIndex() const { return currentWaveIndex_; }
    int GetMaxWaveIndex() const { return static_cast<int>(waves_.size()); }
    bool IsWaveInProgress() const { return isWaveInProgress_; }
    bool IsWaveCleared() const { return isWaveCleared_; }
    bool IsWaveReady() const { return isWaveReady_; }

private:
    void LoadWave(const nlohmann::json &waveJson);
    void SpawnEnemy();

    KashipanEngine::Model *model_;
    KashipanEngine::Model *bulletModel_;
    KashipanEngine::Camera *camera_;

    std::function<void(std::unique_ptr<BaseEntity> enemy)> spawnEnemyCallback_;

    // 現在のウェーブインデックス
    int currentWaveIndex_ = 0;
    // 現在のスポーンインデックス
    int currentSpawnIndex_ = 0;
    // 次のスポーンまでの時間（秒）
    float spawnTime_ = 0.0f;
    // ウェーブ情報のリスト
    std::vector<std::vector<EnemySpawnInfo>> waves_;
    // ウェーブ進行中フラグ
    bool isWaveInProgress_ = false;
    // ウェーブ終了フラグ
    bool isWaveCleared_ = false;
    // ウェーブ準備完了フラグ
    bool isWaveReady_ = true;
};