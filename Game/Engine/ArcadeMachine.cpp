#include "ArcadeMachine.h"

#include "Graphics/Helper.h"
#include "Graphics/ShaderManager.h"
#include "Monitor.h"
#include "Input.h"

void ArcadeMachine::Initialize() {
    stickTransform_.parent = &transform_;
    stickTransform_.translate = { 0.0f, 0.8f, -0.26f };

    body_.CreateFromObj("Resources/Model/body.obj");
    stickRim_.CreateFromObj("Resources/Model/stick_rim.obj");
    stick_.CreateFromObj("Resources/Model/stick.obj");
    board_.CreateFromObj("Resources/Model/board.obj");
}

void ArcadeMachine::Update() {
    auto input = Input::GetInstance();
    auto& xInputState = input->GetXInputState();

    if (input->IsKeyPressed(DIK_D) || input->IsKeyPressed(DIK_A) ||
        std::abs(xInputState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
        Quaternion diff;
        if (input->IsKeyPressed(DIK_D) || xInputState.Gamepad.sThumbLX > 0) {
            diff = Quaternion::MakeForZAxis(-30.0f * Math::ToRadian) * diff;
        }
        if (input->IsKeyPressed(DIK_A) || xInputState.Gamepad.sThumbLX < 0) {
            diff = Quaternion::MakeForZAxis(30.0f * Math::ToRadian) * diff;
        }

        stickTransform_.rotate = Quaternion::Slerp(0.1f, stickTransform_.rotate, diff);
    }
    else {
        stickTransform_.rotate = Quaternion::Slerp(0.1f, stickTransform_.rotate, Quaternion::identity);
    }
}

void ArcadeMachine::Draw(CommandContext& commandContext, const Camera& camera) {
    transform_.UpdateMatrix();
    stickTransform_.UpdateMatrix();

    body_.Draw(commandContext, transform_.worldMatrix, camera);
    stickRim_.Draw(commandContext, transform_.worldMatrix, camera);
    board_.Draw(commandContext, transform_.worldMatrix, camera);
    stick_.Draw(commandContext, stickTransform_.worldMatrix, camera);
    Monitor::GetInstance()->Draw(commandContext, transform_.worldMatrix, camera.GetViewProjectionMatrix());
}

