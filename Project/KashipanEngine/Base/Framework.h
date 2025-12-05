#pragma once
#include <memory>
#include "KashipanEngine.h"

namespace KashipanEngine {

class Framework {
public:
    Framework();
    ~Framework();

    void Initialize(const char *title = "KashipanEngine", int width = 1920, int height = 1080, bool enableDebugLayer = false);
    void Run();
    void Finalize();

private:
    std::unique_ptr<Engine> engine_;
};

} // namespace KashipanEngine
