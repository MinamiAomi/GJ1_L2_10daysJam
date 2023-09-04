#pragma once

#include "../Externals/imgui/imgui.h"

#include "DescriptorHandle.h"

class ImGuiManager {
public:
    static ImGuiManager& Get();

    void Create();
    void NewFrame();
    void Render();
    void Destroy();

public:
    DescriptorHandle descriptorHandle_;

};