#include "CalculateStatusToEnemy.h"
#include <algorithm>

StatusParameters CalculateStatusToEnemy(const StatusParameters &defaultStatus, const StatusParameters &currentStatus) {
    StatusParameters status = currentStatus;
    int level = status.level;
    status.healthMax =
        defaultStatus.healthMax * (1.0f + 0.2f * (level - 1.0f));
    status.exp = 24.0f * (1.0f + 0.2f * (level - 1.0f));
    status.attackPower =
        defaultStatus.attackPower * (1.0f + 0.2f * (level - 1.0f));
    status.bulletShootSpeed =
        defaultStatus.bulletShootSpeed * (1.0f + 0.1f * (level - 1.0f));
    status.bulletShootInterval =
        defaultStatus.bulletShootInterval * std::max(
            1.0f / 30.0f,
            1.0f - 0.05f * (level - 1.0f)
        );
    return status;
}
