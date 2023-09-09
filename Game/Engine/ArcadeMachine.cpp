#include "ArcadeMachine.h"

#include "Graphics/Helper.h"
#include "Graphics/ShaderManager.h"
#include "Monitor.h"

void ArcadeMachine::Initialize() {
    stickTransform_.parent = &transform_;
    stickTransform_.translate = { 0.0f, 0.8f, -0.26f };

    body_.CreateFromObj("Engine/Model/body.obj");
    stickRim_.CreateFromObj("Engine/Model/stick_rim.obj");
    stick_.CreateFromObj("Engine/Model/stick.obj");
}

void ArcadeMachine::Update() {
    
}

void ArcadeMachine::Draw(CommandContext& commandContext, const Matrix4x4& camera) {
    transform_.UpdateMatrix();
    stickTransform_.UpdateMatrix();
    
    body_.Draw(commandContext, transform_.worldMatrix, camera);
    stickRim_.Draw(commandContext, transform_.worldMatrix, camera);
    stick_.Draw(commandContext, stickTransform_.worldMatrix, camera);
    Monitor::GetInstance()->Draw(commandContext, transform_.worldMatrix, camera);
}

