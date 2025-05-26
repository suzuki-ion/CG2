#pragma once

namespace KashipanEngine {

/// @brief リソースリークチェック用構造体
struct D3DResourceLeakChecker {
    ~D3DResourceLeakChecker();
};

} // namespace KashipanEngine