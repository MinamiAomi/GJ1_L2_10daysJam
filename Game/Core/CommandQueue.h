#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

class CommandList;

class CommandQueue {
public:
    ~CommandQueue();

    void Create();
    void ExcuteCommandList(const CommandList& commandList);
    void Signal();
    void WaitForGPU();

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_; }

private:
    void Destroy();

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_;
};