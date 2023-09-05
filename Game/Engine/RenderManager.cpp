#include "RenderManager.h"

#include "Graphics/ShaderManager.h"
#include "GameWindow.h"
#include "ImGuiManager.h"
#include "Graphics/Helper.h"

RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {

    graphics_ = Graphics::GetInstance();
    graphics_->Initialize();


    ShaderManager::GetInstance()->Initialize();

    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    for (auto& commandContext : commandContexts_) {
        commandContext.Create();
        commandContext.Close();
    }

    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
    mainColorBuffer.SetClearColor(clearColor);
    mainColorBuffer.Create(L"SceneColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    bloom.Initialize(&mainColorBuffer);

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHWND(), swapChain_.GetColorBuffer().GetFormat());
}

void RenderManager::BeginRender() {

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->NewFrame();

    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    commandContext.Reset();
    // メインカラーバッファをレンダ―ターゲットに
    commandContext.TransitionResource(mainColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(mainColorBuffer.GetRTV());
    commandContext.ClearColor(mainColorBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, mainColorBuffer.GetWidth(), mainColorBuffer.GetHeight());
}

void RenderManager::EndRender() {
    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    // メインカラーバッファにブルームをかける

    // スワップチェーンをレンダ―ターゲットに
    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext.ClearColor(swapChainBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    // メインカラーバッファをスワップチェーンに移す

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext);

    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandContext.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    commandQueue.WaitForGPU();
    commandQueue.Excute(commandContext);
    swapChain_.Present();
    commandQueue.Signal();
}

void RenderManager::Shutdown() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();
}

void RenderManager::InitializePostEffect() {
    {
        D3D12_DESCRIPTOR_RANGE

    }

}
