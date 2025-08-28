#pragma once

struct StatusParameters {
    int level = 1;
    float exp = 0.0f;
    float expToNextLevel = 100.0f;
    float healthCurrent = 100.0f;
    float healthMax = 100.0f;
    float attackPower = 4.0f;
    float bulletShootSpeed = 8.0f;
    float bulletShootInterval = 1.0f;
};