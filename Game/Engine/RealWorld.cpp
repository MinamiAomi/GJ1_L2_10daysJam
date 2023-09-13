#include "RealWorld.h"

#include "TOMATOsEngine.h"

void RealWorld::Initialize() {
    camera_.SetPosition({ 0.0f, 1.18f, -1.3f });
    camera_.SetRotate(Quaternion::identity);
    //camera_.SetRotate(Quaternion::MakeLookRotation(Vector3(0.0f, -1.0f, 0.1f).Normalized()));
    room_.CreateFromObj("Resources/Model/room.obj");
    room_.SetSamplerType(Model::SamplerType::kPointWrap);
    arcadeMachine_.Initialize();

}

void RealWorld::Update() {
    arcadeMachine_.Update();

    Vector3 position = camera_.GetPosition();
    Quaternion rotate = camera_.GetRotate();

    switch (viewMode_)
    {
    default:
    case RealWorld::ViewMode::kMonitor:
        position = Vector3::Lerp(0.1f, position, { 0.0f, 1.18f, -1.3f });
        rotate = Quaternion::Slerp(0.1f, rotate, Quaternion::identity);
        break;
    case RealWorld::ViewMode::kBoard:
        position = Vector3::Lerp(0.1f, position, { 0.0f, 1.58f, -0.4f });
        rotate = Quaternion::Slerp(0.1f, rotate, Quaternion::MakeLookRotation(Vector3(0.0f, -1.0f, 0.4f).Normalized()));
        break;
    }
    camera_.SetPosition(position);
    camera_.SetRotate(rotate);
}

void RealWorld::Draw(CommandContext& commandContext) {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    Vector3 pos = camera_.GetPosition();
    ImGui::DragFloat3("Pos", &pos.x, 0.01f);
    camera_.SetPosition(pos);
    ImGui::End();
#endif // _DEBUG


    camera_.UpdateMatrices();
    room_.Draw(commandContext, Matrix4x4::identity, camera_, false);
    arcadeMachine_.Draw(commandContext, camera_);
}
