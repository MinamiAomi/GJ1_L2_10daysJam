#include "CommandList.h"

#include "Externals/DirectXTex/d3dx12.h"

#include <cassert>

#include "Utility.h"
#include "Graphics.h"
#include "CommandQueue.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "GPUResource.h"
#include "DescriptorHeap.h"

void CommandList::Create() {
    auto device = Graphics::Get().GetDevice();

    ASSERT_IF_FAILED(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.ReleaseAndGetAddressOf())));

    ASSERT_IF_FAILED(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
        nullptr, IID_PPV_ARGS(commandList_.ReleaseAndGetAddressOf())));

    isRecording_ = true;
    Close();
}

void CommandList::Close() {
    assert(isRecording_);
    ASSERT_IF_FAILED(commandList_->Close());
    isRecording_ = false;
}

void CommandList::Reset() {
    assert(!isRecording_);
    ASSERT_IF_FAILED(commandAllocator_->Reset());
    ASSERT_IF_FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr));
    isRecording_ = true;

    BindDescriptorHeaps();
    graphicsRootSignature_ = nullptr;
    pipelineState_ = nullptr;
}

void CommandList::TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {
    resourceBarriers_.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, stateBefore, stateAfter));
}

void CommandList::TransitionBarrier(GPUResource& resource, D3D12_RESOURCE_STATES stateAfter) {
    if (resource.GetCurrentState() != stateAfter) {
        resourceBarriers_.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource.Get().Get(), resource.GetCurrentState(), stateAfter));
        resource.currentState_ = stateAfter;
    }
}

void CommandList::CommitResourceBarrier() {
    if (!resourceBarriers_.empty()) {
        commandList_->ResourceBarrier(UINT(resourceBarriers_.size()), resourceBarriers_.data());
        resourceBarriers_.clear();
    }
}

void CommandList::CopyResource(const GPUResource& dst, const GPUResource& src) {
    commandList_->CopyResource(dst.Get().Get(), src.Get().Get());
}

void CommandList::SetGraphicsRootSignature(const RootSignature& rootSignature) {
    auto r = rootSignature.GetRootSignature();
    if (graphicsRootSignature_ != r.Get()) {
        graphicsRootSignature_ = r.Get();
        commandList_->SetGraphicsRootSignature(graphicsRootSignature_);
    }
}

void CommandList::SetGraphicsRootCBV(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) {
    commandList_->SetGraphicsRootConstantBufferView(rootParameterIndex, bufferLocation);
}

void CommandList::SetGraphicsRootSRV(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) {
    commandList_->SetGraphicsRootShaderResourceView(rootParameterIndex, bufferLocation);
}

void CommandList::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
    commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
}

void CommandList::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHandle& baseDescriptor) {
    commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, baseDescriptor.GPU());
}

void CommandList::SetPipelineState(const PipelineState& pipelineState) {
    auto p = pipelineState.GetPipelineState();
    if (pipelineState_ != p.Get()) {
        pipelineState_ = p.Get();
        commandList_->SetPipelineState(pipelineState_);
    }
}

void CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, const DescriptorHeap& descriptorHeap) {
    auto descHeap = descriptorHeap.GetDescriptorHeap();
    if (descriptorHeaps_[type] != descHeap.Get()) {
        descriptorHeaps_[type] = descHeap.Get();
        BindDescriptorHeaps();
    }
}

void CommandList::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]) {
    commandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, nullptr);
}

void CommandList::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv) {
    commandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, &dsv);
}

void CommandList::ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float clearColor[4]) {
    commandList_->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void CommandList::ClearColor(const ColorBuffer& colorBuffer) {
    float clearColor[4]{ 0.0f,0.0f,0.0f,0.0f };
    if (colorBuffer.GetClearValue()) {
        memcpy(clearColor, colorBuffer.GetClearValue()->Color, sizeof(clearColor));
    }
    ClearColor(colorBuffer.GetRenderTargetView().CPU(), clearColor);
}

void CommandList::ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, float depth) {
    commandList_->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void CommandList::ClearDepth(const DepthBuffer& depthBuffer) {
    ClearDepth(depthBuffer.GetDepthStencilView().CPU(), depthBuffer.GetClearValue()->DepthStencil.Depth);
}

void CommandList::SetViewport(const D3D12_VIEWPORT& viewport) {
    commandList_->RSSetViewports(1, &viewport);
}

void CommandList::SetViewport(float leftTopX, float leftTopY, float width, float height, float minDepth, float maxDepth) {
    SetViewport(CD3DX12_VIEWPORT(leftTopX, leftTopY, width, height, minDepth, maxDepth));
}

void CommandList::SetScissor(const D3D12_RECT& rect) {
    commandList_->RSSetScissorRects(1, &rect);
}

void CommandList::SetScissor(UINT left, UINT top, UINT right, UINT bottom) {
    SetScissor(CD3DX12_RECT(left, top, right, bottom));
}

void CommandList::SetViewportAndScissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
    SetViewport(viewport);
    SetScissor(rect);
}

void CommandList::SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height) {
    SetViewport(float(x), float(y), float(width), float(height));
    SetScissor(x, y, x + width, y + height);
}

void CommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) {
    commandList_->IASetPrimitiveTopology(topology);
}

void CommandList::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vbv, UINT slot) {
    commandList_->IASetVertexBuffers(slot, 1, &vbv);
}

void CommandList::SetVertexBuffers(UINT numVBVs, const D3D12_VERTEX_BUFFER_VIEW vbv[], UINT startSlot) {
    commandList_->IASetVertexBuffers(startSlot, numVBVs, vbv);
}

void CommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv) {
    commandList_->IASetIndexBuffer(&ibv);
}

void CommandList::Draw(UINT vertexCount, UINT startVertexLocation) {
    commandList_->DrawInstanced(vertexCount, 1, startVertexLocation, 0);
}

void CommandList::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) {
    commandList_->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

void CommandList::BindDescriptorHeaps() {
    UINT numDescriptorHeaps = 0;
    ID3D12DescriptorHeap* ppHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ nullptr };
    for (auto iter : descriptorHeaps_) {
        if (iter != nullptr) {
            ppHeaps[numDescriptorHeaps++] = iter;
        }
    }

    if (numDescriptorHeaps > 0) {
        commandList_->SetDescriptorHeaps(numDescriptorHeaps, ppHeaps);
    }
}
