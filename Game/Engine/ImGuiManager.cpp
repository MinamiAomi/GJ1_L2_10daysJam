#include "ImGuiManager.h"

#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"

#include "Graphics/Graphics.h"
#include "Graphics/SwapChain.h"
#include "Graphics/CommandContext.h"

ImGuiManager* ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return &instance;
}

void ImGuiManager::Initialize(HWND hWnd, DXGI_FORMAT rtvFormat) {
    auto graphics = Graphics::GetInstance();

    auto descriptor = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(
        graphics->GetDevice(),
        SwapChain::kNumBuffers,
        rtvFormat,
        graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        descriptor,
        descriptor);
}

void ImGuiManager::NewFrame() {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render(CommandContext& commandContext) {
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContext);
}

void ImGuiManager::Shutdown() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
