#pragma once

#include <d3d12.h>

#include <cstdint>

class DescriptorHandle {
    friend class DescriptorHeap;
public:
    bool IsShaderVisible() const { return gpu_.ptr != 0; }
    bool IsNull() const { return cpu_.ptr == 0; }
    D3D12_CPU_DESCRIPTOR_HANDLE CPU() const { return cpu_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GPU() const { return gpu_; }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_{ 0 };
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_{ 0 };
};