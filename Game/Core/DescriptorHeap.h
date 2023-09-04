#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "DescriptorHandle.h"

class DescriptorHeap {
public:

    void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

    DescriptorHandle Allocate();
    void Reset(uint32_t reallocationOffset = 0);

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return descriptorHeap_; }

private:
    D3D12_DESCRIPTOR_HEAP_TYPE type_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    DescriptorHandle descriptorStart_;
    uint32_t numDescriptors_;
    uint32_t descriptorSize_;
    uint32_t freeDescriptorOffset_;
};