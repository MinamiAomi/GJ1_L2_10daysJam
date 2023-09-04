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
    void Render();

    SwapChain& GetSwapChain() { return swapChain_; }

private:
    RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContexts_[SwapChain::kNumBuffers];

    ColorBuffer sceneColorBuffer;
    Bloom bloom;

};