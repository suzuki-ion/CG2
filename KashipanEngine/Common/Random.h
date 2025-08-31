#pragma once

namespace KashipanEngine {

/// @brief 乱数初期化用関数
void InitializeRandom();

/// @brief 乱数の生成
/// @return int型の最小値以上最大値以下の乱数
int GetRandomInt();

/// @brief 乱数の生成
/// @return float型の最小値以上最大値以下の乱数
float GetRandomFloat();

/// @brief 乱数の生成
/// @param min 最小値
/// @param max 最大値
/// @return min以上max以下の乱数
int GetRandomInt(int min, int max);

/// @brief 乱数の生成
/// @param min 最小値
/// @param max 最大値
/// @return min以上max以下の乱数
float GetRandomFloat(float min, float max);

} // namespace KashipanEngine