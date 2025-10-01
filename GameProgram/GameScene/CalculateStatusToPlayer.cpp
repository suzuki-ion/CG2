#include "CalculateStatusToPlayer.h"
#include <algorithm>
#include <cmath>

StatusParameters CalculateStatusToPlayer(const StatusParameters &defaultStatus, const StatusParameters &currentStatus) {
    StatusParameters status = currentStatus;
    float level = static_cast<float>(status.level);
    status.healthMax =
        defaultStatus.healthMax * (1.0f + 0.1f * (level - 1.0f));
    status.expToNextLevel = 50.0f * std::pow(level, 1.5f);
    status.attackPower =
        defaultStatus.attackPower * (1.0f + 0.1f * (level - 1.0f));
    status.bulletShootSpeed =
        defaultStatus.bulletShootSpeed * (1.0f + 0.05f * (level - 1.0f));
    status.bulletShootInterval =
        defaultStatus.bulletShootInterval * std::max(
            1.0f / 60.0f,
            1.0f - 0.05f * (level - 1.0f)
        );
    return status;
}
