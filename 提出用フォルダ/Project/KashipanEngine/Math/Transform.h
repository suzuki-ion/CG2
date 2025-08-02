#pragma once
#include "Vector3.h"

namespace KashipanEngine {

struct Transform {
    Vector3 scale = { 1.0f, 1.0f, 1.0f };
    Vector3 rotate = { 0.0f, 0.0f, 0.0f };
    Vector3 translate = { 0.0f, 0.0f, 0.0f };
};

}