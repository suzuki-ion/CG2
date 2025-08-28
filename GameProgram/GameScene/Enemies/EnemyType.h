#pragma once
#include <unordered_map>
#include <string>

enum class EnemyType {
    None,
    Normal,
    StopMidway,
    Boss
};

const std::unordered_map<std::string, EnemyType> kEnemyTypeMap = {
    { "None", EnemyType::None },
    { "Normal", EnemyType::Normal },
    { "StopMidway", EnemyType::StopMidway },
    { "Boss", EnemyType::Boss }
};