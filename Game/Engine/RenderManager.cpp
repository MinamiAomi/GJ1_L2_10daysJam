#include "RenderManager.h"

#include "Graphics/ShaderManager.h"
#include "GameWindow.h"

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
    sceneColorBuffer.SetClearColor(clearColor);
    sceneColorBuffer.Create(L"SceneColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    bloom.Initialize(&sceneColorBuffer);
}

void RenderManager::Render() {
    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    commandContext.Reset();



    commandContext.TransitionResource(sceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(sceneColorBuffer.GetRTV());
    commandContext.ClearColor(sceneColorBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, sceneColorBuffer.GetWidth(), sceneColorBuffer.GetHeight());



    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext.ClearColor(swapChainBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());



    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandContext.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    commandQueue.WaitForGPU();
    commandQueue.Excute(commandContext);
    swapChain_.Present();
    commandQueue.Signal();
}
