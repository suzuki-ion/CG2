#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Sprite : public Object {
    Sprite() {
        mesh = PrimitiveDrawer::CreateMesh(4, 6);
        mesh->indexBufferMap[0] = 0;
        mesh->indexBufferMap[1] = 1;
        mesh->indexBufferMap[2] = 2;
        mesh->indexBufferMap[3] = 1;
        mesh->indexBufferMap[4] = 3;
        mesh->indexBufferMap[5] = 2;
    }
};

} // namespace KashipanEngine