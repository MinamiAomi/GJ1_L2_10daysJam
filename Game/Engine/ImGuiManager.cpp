#include "ImGuiManager.h"

#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"

#include "GameWindow.h"

ImGuiManager* ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return &instance;
}

void ImGuiManager::Initialize() {
    auto window = GameWindow::GetInstance();
    auto graphics = Graphics::GetInstance();

    auto descriptor = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window->GetHWND());
    ImGui_ImplDX12_Init(
        graphics->GetDevice(),
        SwapChain::kNumBuffers,
        graphics.GetCurrentSwapChainColorBuffer().GetRTVFormat(),
        graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        descriptor,
        descriptor);
}

void ImGuiManager::NewFrame()
{
}

void ImGuiManager::Render(CommandContext& commandContext)
{
}

void ImGuiManager::Terminate()
{
}
