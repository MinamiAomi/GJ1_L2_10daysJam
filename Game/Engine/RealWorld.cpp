#include "RealWorld.h"

void RealWorld::Initialize() {
    camera_.SetPosition({ 0.0f, 1.2f, -1.5f });
    camera_.SetRotate(Quaternion::identity);
    arcadeMachine_.Initialize();

}

void RealWorld::Update() {
    arcadeMachine_.Update();
}

void RealWorld::Draw(CommandContext& commandContext) {
    camera_.UpdateMatrices();
    arcadeMachine_.Draw(commandContext, camera_.GetViewProjectionMatrix());
}
