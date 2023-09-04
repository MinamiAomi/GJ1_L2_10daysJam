#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHandle.h"

class CommandQueue;
class CommandList;
class DescriptorHeap;
class ColorBuffer;
class DepthBuffer;

class Graphics {
public:
    static const uint32_t kNumSwapChainBuffers = 2;

    static Graphics& Get();
    
    void Create();

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
    
    void Render();

    CommandQueue& GetCommandQueue() const { return *commandQueue_; }
    CommandList& GetCommandList() const { return *commandList_; }
    DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const { return *descriptorHeaps_[type]; }

    ColorBuffer& GetCurrentSwapChainColorBuffer() const { return *swapChainColorBuffers_[swapChain_->GetCurrentBackBufferIndex()]; }
    DepthBuffer& GetSwapChainDepthBuffer() const { return *swapChainDepthBuffer_; }

private:
    static const uint32_t kNumRTVDescriptors = 4;
    static const uint32_t kNumDSVDescriptors = 2;
    static const uint32_t kNumSRVDescriptors = 256;
    static const uint32_t kNumSmpalerDescriptors = 1;

    Graphics() = default;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;


    void CreateDevice();
    void CreateSwapChain();

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    
    std::unique_ptr<CommandQueue> commandQueue_;
    std::unique_ptr<CommandList> commandList_;
    std::unique_ptr<DescriptorHeap> descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    std::unique_ptr<ColorBuffer> swapChainColorBuffers_[kNumSwapChainBuffers];
    std::unique_ptr<DepthBuffer> swapChainDepthBuffer_;
};
