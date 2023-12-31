#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/SwapChain.h"
#include "Graphics/CommandContext.h"
#include "Graphics/ColorBuffer.h"
#include "Graphics/Bloom.h"

class RenderManager {
public:

    static RenderManager* GetInstance();

    void Initialize();
    void BeginRender();
    void EndRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return swapChain_; }
    CommandContext& GetCommandContext() { return commandContexts_[swapChain_.GetBufferIndex()]; }
    Bloom& GetBloom() { return bloom; }

    DXGI_FORMAT GetSwapChainRTVFormat() const { return swapChain_.GetColorBuffer().GetFormat(); }
    DXGI_FORMAT GetMainBufferRTVFormat() const { return mainColorBuffer.GetFormat(); }

    

private:
    RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    void InitializePostEffect();

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContexts_[SwapChain::kNumBuffers];

    ColorBuffer mainColorBuffer;
    RootSignature postEffectRootSignature_;
    PipelineState postEffectPipelineState_;
    Bloom bloom;
};