#pragma once

#include "Math/Camera.h"
#include "Math/MathUtils.h"
#include "Model.h"
#include "ArcadeMachine.h"

class CommandContext;

class RealWorld {
public:
    enum class ViewMode {
        kMonitor,
        kBoard
    };

    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext);

    void SetViewMode(ViewMode viewMode) { viewMode_ = viewMode; }

private:
    Camera camera_;
    Model room_;
    ArcadeMachine arcadeMachine_;

    ViewMode viewMode_ = ViewMode::kMonitor;
};