#include "ImGuiManager.h"

#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"

#include "Window.h"
#include "Graphics.h"
#include "GPUResource.h"
#include "CommandList.h"

ImGuiManager& ImGuiManager::Get() {
    static ImGuiManager instance;
    return instance;
}

void ImGuiManager::Create() {
    auto& window = Window::Get();
    auto& graphics = Graphics::Get();

    descriptorHandle_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window.GetHWND());
    ImGui_ImplDX12_Init(
        graphics.GetDevice().Get(),
        Graphics::kNumSwapChainBuffers,
        graphics.GetCurrentSwapChainColorBuffer().GetRTVFormat(),
        graphics.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetDescriptorHeap().Get(),
        descriptorHandle_.CPU(),
        descriptorHandle_.GPU());
}

void ImGuiManager::NewFrame() {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    auto& commandList = Graphics::Get().GetCommandList();
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.GetCommandList().Get());
}

void ImGuiManager::Destroy() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
