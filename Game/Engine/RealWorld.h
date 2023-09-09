#pragma once

#include "Math/Camera.h"
#include "Math/MathUtils.h"
#include "Monitor.h"

class CommandContext;

class RealWorld {
public:

    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext);

    Monitor& GetMonitor() { return monitor_; }

private:
    Camera camera_;
    Monitor monitor_;


};