#pragma once
#include <Objects/Cube.h>

class LaserPointer {
public:
    LaserPointer();
    ~LaserPointer() = default;

    void SetPosition(const KashipanEngine::Vector3 &position) {
        worldTransform_->translate_ = position;
    }
    void SetLength(float length) {
        worldTransform_->scale_.z = length;
    }
    float GetLength() const {
        return worldTransform_->scale_.z;
    }
    void Draw();

private:
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    std::unique_ptr<KashipanEngine::Cube> laserPointerCube_;
};