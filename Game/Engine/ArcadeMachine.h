#pragma once

#include <memory>
#include <vector>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"
#include "Model.h"


class ArcadeMachine {
public:
    void Initialize();
    void Update();
    void Draw(CommandContext& commandContext, const Camera& camera);

private:
    Transform transform_;
    Transform stickTransform_;

    Model body_;
    Model stick_;
    Model stickRim_;
    Model board_;

};