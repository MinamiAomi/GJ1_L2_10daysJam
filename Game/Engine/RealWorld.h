#pragma once

#include "Math/Camera.h"
#include "Math/MathUtils.h"
#include "Model.h"
#include "ArcadeMachine.h"

class CommandContext;

class RealWorld {
public:

    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext);

private:
    Camera camera_;
    Model room_;
    ArcadeMachine arcadeMachine_;

};