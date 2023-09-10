#pragma once

#include "Math/Camera.h"
#include "Math/MathUtils.h"

#include "ArcadeMachine.h"

class CommandContext;

class RealWorld {
public:

    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext);

private:
    Camera camera_;
    ArcadeMachine arcadeMachine_;

};