#pragma once

#include <memory>
#include <vector>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Model.h"


class ArcadeMachine {
public:
    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext, const Matrix4x4& camera);

private:
    Transform transform_;
    Transform stickTransform_;

    Model body_;
    Model stick_;
    Model stickRim_;

};