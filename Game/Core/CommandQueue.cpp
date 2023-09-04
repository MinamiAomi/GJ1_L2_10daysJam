#include "CommandQueue.h"

#include "Utility.h"
#include "Graphics.h"
#include "CommandList.h"

#include <assert.h>

CommandQueue::~CommandQueue() {
    Destroy();
}

void CommandQueue::Destroy() {
    WaitForGPU();
}

void CommandQueue::Create() {
    auto device = Graphics::Get().GetDevice();

    D3D12_COMMAND_QUEUE_DESC desc{};
    ASSERT_IF_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())));

    ASSERT_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())));

    if (fenceEvent_) {
        fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(fenceEvent_);
    }

    fenceValue_ = 0;
}

void CommandQueue::ExcuteCommandList(const CommandList& commandList) {
    ID3D12CommandList* ppCmdList[] = { commandList.GetCommandList().Get() };
    commandQueue_->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);
}

void CommandQueue::Signal() {
    ASSERT_IF_FAILED(commandQueue_->Signal(fence_.Get(), ++fenceValue_));
}

void CommandQueue::WaitForGPU() {
    if (fence_->GetCompletedValue() < fenceValue_) {
        ASSERT_IF_FAILED(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}
