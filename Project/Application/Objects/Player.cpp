#include "Player.h"
#include <cmath>

namespace KashipanEngine {

Player::Player(const Vector3 &cameraRotate, int textureIndex) {
    plane_ = std::make_unique<Plane>();
    {
        auto st = plane_->GetStatePtr();
        st.transform->scale = {1.0f, 1.0f, 1.0f};
        st.transform->translate = {0.0f, st.transform->scale.y * 0.5f, 0.0f};
        st.transform->rotate = -cameraRotate;
        *st.useTextureIndex = textureIndex;
        st.material->lightingType = 0;
    }
    plane_->SetPipelineName("Object3d.Solid.BlendNormal");
}

void Player::Update() {
    Vector3 move = {0.0f, 0.0f, 0.0f};
    if (Input::IsKeyDown(DIK_W)) { move.z += 1.0f; }
    if (Input::IsKeyDown(DIK_S)) { move.z -= 1.0f; }
    if (Input::IsKeyDown(DIK_A)) { move.x -= 1.0f; }
    if (Input::IsKeyDown(DIK_D)) { move.x += 1.0f; }

    auto planeState = plane_->GetStatePtr();
    planeState.transform->translate += move.Normalize() * moveSpeed_;
    planeState.transform->translate.y = planeState.transform->scale.y * 0.5f;
}

void Player::Draw() {
    plane_->Draw();
}

Vector3 Player::GetPosition() const {
    auto st = plane_->GetStatePtr();
    return st.transform->translate;
}

} // namespace KashipanEngine
