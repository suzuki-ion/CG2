#pragma once
#include <unordered_map>
#include <string>

enum class EnemyType {
    None,
    Normal,
    Fixed,
    Boss
};

const std::unordered_map<std::string, EnemyType> kEnemyTypeMap = {
    { "None", EnemyType::None },
    { "Normal", EnemyType::Normal },
    { "Fixed", EnemyType::Fixed },
    { "Boss", EnemyType::Boss }
};