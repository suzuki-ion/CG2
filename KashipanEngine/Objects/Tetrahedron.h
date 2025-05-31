#pragma once
#include "Object.h"

namespace KashipanEngine {

class Tetrahedron : public Object {
public:
    Tetrahedron();

    /// @brief 描画処理
    void Draw();
};

} // namespace KashipanEngine
