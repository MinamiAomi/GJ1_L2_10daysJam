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

    camera_.UpdateMatrices();
    room_.Draw(commandContext, Matrix4x4::identity, camera_.GetViewProjectionMatrix());
    arcadeMachine_.Draw(commandContext, camera_.GetViewProjectionMatrix());
}
