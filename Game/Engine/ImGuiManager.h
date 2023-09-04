#pragma once

#include "Externals/ImGui/imgui.h"

class CommandContext;

class ImGuiManager {
public:
    static ImGuiManager* GetInstance();

    void Initialize(HWND hWnd, ID3D12Device* device, uint32_t numFrames, DXGI_FORMAT rtvFormat, DescriptorHeap& descriptorHeap);
    void NewFrame();
    void Render(CommandContext& commandContext);
    void Terminate();

private:
    ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
};