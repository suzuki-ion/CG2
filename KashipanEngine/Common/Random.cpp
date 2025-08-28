#include <random>
#include <limits>
#include "Random.h"

namespace KashipanEngine {

namespace {
std::mt19937 randomEngine;
} // namespace

void InitializeRandom() {
    std::random_device rd;
    randomEngine.seed(rd());
}

int GetRandomInt() {
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    );
    return dist(randomEngine);
}

float GetRandomFloat() {
    std::uniform_real_distribution<float> dist(
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max()
    );
    return dist(randomEngine);
}

int GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(randomEngine);
}

float GetRandomFloat (float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomEngine);
}

} // namespace KashipanEngine