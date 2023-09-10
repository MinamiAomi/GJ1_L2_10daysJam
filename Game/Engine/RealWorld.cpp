#include "RealWorld.h"

#include "TOMATOsEngine.h"

void RealWorld::Initialize() {
    camera_.SetPosition({ 0.0f, 1.2f, -1.3f });
    camera_.SetRotate(Quaternion::identity);
    room_.CreateFromObj("Engine/Model/room.obj");
    arcadeMachine_.Initialize();

}

void RealWorld::Update() {
    arcadeMachine_.Update();
}

void RealWorld::Draw(CommandContext& commandContext) {

    ImGui::Begin("Camera");
    Vector3 pos = camera_.GetPosition();
    ImGui::DragFloat3("Pos", &pos.x, 0.01f);
    camera_.SetPosition(pos);
    ImGui::End();

    camera_.UpdateMatrices();
    room_.Draw(commandContext, Matrix4x4::identity, camera_.GetViewProjectionMatrix(), false);
    arcadeMachine_.Draw(commandContext, camera_.GetViewProjectionMatrix());
}
