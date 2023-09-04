#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHandle.h"

class ColorBuffer;

class SwapChain {
public:
    static const uint32_t kNumBuffers = 2;

    static SwapChain* GetInstance();

    void Initialize();
    void Present();

    ColorBuffer& GetColorBuffer() { return *buffers_[currentBufferIndex_]; }

private:
    SwapChain() = default;
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    std::unique_ptr<ColorBuffer> buffers_[kNumBuffers];
    uint32_t currentBufferIndex_ = 0;
};