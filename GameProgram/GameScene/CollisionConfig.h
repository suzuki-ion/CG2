#pragma once
#include <bitset>

// プレイヤー陣営
const std::bitset<8> kCollisionAttributePlayer  = 0b00000001;
// 敵陣営
const std::bitset<8> kCollisionAttributeEnemy   = 0b00000010;