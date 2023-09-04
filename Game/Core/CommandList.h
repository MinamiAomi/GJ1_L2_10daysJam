#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

class CommandQueue;
class GPUResource;
class RootSignature;
class PipelineState;
class DescriptorHeap;
class ColorBuffer;
class DepthBuffer;
class DescriptorHandle;

class CommandList {
public:
    void Create();

    void Close();
    void Reset();

    void TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
    void TransitionBarrier(GPUResource& resource, D3D12_RESOURCE_STATES stateAfter);
    void CommitResourceBarrier();

    void CopyResource(const GPUResource& dst, const GPUResource& src);

    void SetPipelineState(const PipelineState& pipelineState);
    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, const DescriptorHeap& descriptorHeap);
    void SetGraphicsRootSignature(const RootSignature& rootSignature);
    void SetGraphicsRootCBV(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
    void SetGraphicsRootSRV(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
    void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
    void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const DescriptorHandle& baseDescriptor);

    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]);
    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv);
    void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }
    void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv) { SetRenderTargets(1, &rtv); }

    void ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float clearColor[4]);
    void ClearColor(const ColorBuffer& colorBuffer);
    void ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, float depth = 1.0f);
    void ClearDepth(const DepthBuffer& depthBuffer);

    void SetViewport(const D3D12_VIEWPORT& viewport);
    void SetViewport(float leftTopX, float leftTopY, float width, float height, float minDepth = D3D12_MIN_DEPTH, float maxDepth = D3D12_MAX_DEPTH);
    void SetScissor(const D3D12_RECT& rect);
    void SetScissor(UINT left, UINT top, UINT right, UINT bottom);
    void SetViewportAndScissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect);
    void SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height);

    void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vbv, UINT slot = 0);
    void SetVertexBuffers(UINT numVBVs, const D3D12_VERTEX_BUFFER_VIEW vbv[], UINT startSlot = 0);
    void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv);
    void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);

    void Draw(UINT vertexCount, UINT startVertexLocation = 0);
    void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList_; }

private:
    void BindDescriptorHeaps();

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    bool isRecording_;

    std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers_;

    ID3D12RootSignature* graphicsRootSignature_;
    ID3D12PipelineState* pipelineState_;
    ID3D12DescriptorHeap* descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};
