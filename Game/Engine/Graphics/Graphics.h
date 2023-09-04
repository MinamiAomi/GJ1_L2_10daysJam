#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHandle.h"

class DescriptorHeap;

class Graphics {
public:
    static Graphics* GetInstance();

    void Initialize();
    void Terminate();

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

    ID3D12Device* GetDevice() const { return device_.Get(); }
    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }
    DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const { return descriptorHeaps_[type].get(); }

private:
    static const uint32_t kNumRTVs = 8;
    static const uint32_t kNumDSVs = 1;
    static const uint32_t kNumSRVs = 256;
    static const uint32_t kNumSamplers = 1;

    Graphics() = default;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void CreateDevice();

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

    std::unique_ptr<DescriptorHeap> descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    
};